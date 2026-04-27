###############################################################################
# Stage 1 — Download external tooling that can be fetched in parallel and
#            discarded after extracting.  Keeps the final image free of
#            zip/tar archives and download utilities.
###############################################################################
FROM ubuntu:24.04 AS downloads

ARG DEBIAN_FRONTEND=noninteractive
ARG EIM_VERSION=v0.7.1
ARG QEMU_REL=esp_develop_9.0.0_20240606
ARG QEMU_SHA256=071d117c44a6e9a1bc8664ab63b592d3e17ceb779119dcb46c59571a4a7a88c9

RUN apt-get update \
    && apt-get install -y --no-install-recommends curl unzip wget xz-utils ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# -- EIM CLI (ESP-IDF Install Manager) ------------------------------------
RUN curl -fsSL "https://github.com/espressif/idf-im-ui/releases/download/${EIM_VERSION}/eim-cli-linux-x64.zip" \
        -o /tmp/eim-cli.zip \
    && unzip /tmp/eim-cli.zip -d /tmp/eim \
    && find /tmp/eim -name "eim" -type f -exec cp {} /usr/local/bin/eim \; \
    && chmod +x /usr/local/bin/eim \
    && rm -rf /tmp/eim-cli.zip /tmp/eim

# -- AWS CLI v2 -----------------------------------------------------------
RUN curl -fsSL "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o /tmp/awscliv2.zip \
    && unzip /tmp/awscliv2.zip -d /tmp \
    && /tmp/aws/install --install-dir /opt/aws-cli --bin-dir /usr/local/bin \
    && rm -rf /tmp/awscliv2.zip /tmp/aws

# -- QEMU for ESP32 emulation ---------------------------------------------
RUN wget --no-verbose \
        "https://github.com/espressif/qemu/releases/download/esp-develop-9.0.0-20240606/qemu-xtensa-softmmu-${QEMU_REL}-x86_64-linux-gnu.tar.xz" \
        -O /tmp/qemu.tar.xz \
    && echo "${QEMU_SHA256} */tmp/qemu.tar.xz" | sha256sum --check --strict - \
    && tar -xf /tmp/qemu.tar.xz -C /opt \
    && rm /tmp/qemu.tar.xz


###############################################################################
# Stage 2 — Final development image
###############################################################################
FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTAINER_USER=ubuntu
ARG IDF_VERSION=v5.4.2

# ## System dependencies ##################################################
# Includes prerequisites for ESP-IDF (git, python3, cmake, ninja, etc.),
# Bluetooth/WiFi stack, and E2E testing tools.
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        sudo \
        vim \
        build-essential \
        cmake \
        ninja-build \
        git \
        bash-completion \
        curl \
        python3 \
        python3-pip \
        python3-venv \
        libglib2.0-0 \
        libnuma1 \
        libpixman-1-0 \
        bluez \
        bluez-tools \
        dbus \
        dfu-util \
        libssl-dev \
        udev \
        libusb-1.0-0 \
        network-manager \
        openssh-client \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# ## User setup ############################################################
RUN id -u "${CONTAINER_USER}" > /dev/null 2>&1 || useradd -m -s /bin/bash "${CONTAINER_USER}" \
    && echo "${CONTAINER_USER} ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/"${CONTAINER_USER}" \
    && chmod 0440 /etc/sudoers.d/"${CONTAINER_USER}"

# ## Copy artifacts from downloads stage ###################################
COPY --from=downloads /usr/local/bin/eim /usr/local/bin/eim
COPY --from=downloads /opt/aws-cli /opt/aws-cli
COPY --from=downloads /usr/local/bin/aws /usr/local/bin/aws
COPY --from=downloads /opt/qemu /opt/qemu

# ## Install ESP-IDF via EIM CLI ###########################################
# EIM installs:
#   - ESP-IDF source into  -p <path>/<version>/esp-idf  → /opt/esp/<version>/esp-idf
#   - Tools & venv into    ~/.espressif/tools/           → /root/.espressif/tools/
#
# After installation we:
#   1. Move tools from /root/.espressif/tools → /opt/esp/tools  (accessible to all users)
#   2. Patch all path references (activation script + eim_idf.json)
#   3. Create convenience symlinks and wrapper scripts
#   4. Keep the EIM binary so the VS Code extension's eimPath check passes
RUN eim install \
        -p /opt/esp \
        -i "${IDF_VERSION}" \
        -t esp32s3 \
        --skip-prerequisites-check true \
    # --- Relocate tools from /root/.espressif to /opt/esp -----------------
    && mv /root/.espressif/tools /opt/esp/tools \
    && rmdir /root/.espressif 2>/dev/null || true \
    && sed -i "s|/root/.espressif/tools|/opt/esp/tools|g" \
        /opt/esp/tools/activate_idf_${IDF_VERSION}.sh \
        /opt/esp/tools/eim_idf.json \
        /opt/esp/tools/python/${IDF_VERSION}/venv/pyvenv.cfg \
    && find /opt/esp/tools/python/${IDF_VERSION}/venv/bin -type f \
        -exec sed -i "s|/root/.espressif/tools|/opt/esp/tools|g" {} + \
    # --- Convenience symlinks ---------------------------------------------
    && ln -sf /opt/esp/${IDF_VERSION}/esp-idf /opt/esp/idf \
    && ln -sf /opt/esp/tools/python/${IDF_VERSION}/venv /opt/esp/python \
    && ln -sf /opt/esp/tools/activate_idf_${IDF_VERSION}.sh /opt/esp/activate.sh \
    # --- Wrapper scripts for non-interactive contexts ---------------------
    && printf '#!/bin/sh\nexec /opt/esp/tools/python/%s/venv/bin/python3 /opt/esp/%s/esp-idf/tools/idf.py "$@"\n' \
        "${IDF_VERSION}" "${IDF_VERSION}" > /usr/local/bin/idf.py \
    && printf '#!/bin/sh\nexec /opt/esp/tools/python/%s/venv/bin/python3 /opt/esp/%s/esp-idf/components/esptool_py/esptool/esptool.py "$@"\n' \
        "${IDF_VERSION}" "${IDF_VERSION}" > /usr/local/bin/esptool.py \
    && chmod +x /usr/local/bin/idf.py /usr/local/bin/esptool.py \
    # --- Permissions & git safe directory ---------------------------------
    && chmod -R a+rX /opt/esp \
    && git config --system --add safe.directory /opt/esp/${IDF_VERSION}/esp-idf

# ## Install extra Python dependencies into EIM's venv ####################
# Use ESP-IDF's constraint file to prevent overwriting pinned dependencies
# (e.g. esptool) with incompatible versions pulled in by other packages.
COPY requirements.txt /tmp/
RUN . /opt/esp/python/bin/activate \
    && pip install --no-cache-dir --upgrade pip \
    && pip install --no-cache-dir \
        -c /opt/esp/tools/espidf.constraints.v5.4.txt \
        -r /tmp/requirements.txt \
    && rm /tmp/requirements.txt

# ## Environment ###########################################################
ENV LC_ALL=C.UTF-8 \
    LANG=C.UTF-8 \
    PATH="/home/ubuntu/.local/bin:/opt/qemu/bin:${PATH}"

# ## Switch to dev user ####################################################
USER ${CONTAINER_USER}
ENV USER=${CONTAINER_USER}
WORKDIR /home/${CONTAINER_USER}

# Place eim_idf.json where the VS Code extension expects it
# (hardcoded: $HOME/.espressif/tools/eim_idf.json)
RUN mkdir -p /home/${CONTAINER_USER}/.espressif/tools \
    && cp /opt/esp/tools/eim_idf.json /home/${CONTAINER_USER}/.espressif/tools/eim_idf.json

# ## Install Claude Code (native installer) ################################
RUN curl -fsSL https://claude.ai/install.sh | bash

# ## Shell configuration ###################################################
RUN mkdir -p /home/${CONTAINER_USER}/.shell_history \
    && { \
        echo 'source /opt/esp/activate.sh > /dev/null 2>&1'; \
        echo 'export PATH="$HOME/.local/bin:$PATH"'; \
        echo 'export HISTFILE="$HOME/.shell_history/.bash_history"'; \
        echo 'export HISTSIZE=10000'; \
        echo 'export HISTFILESIZE=20000'; \
    } >> /home/${CONTAINER_USER}/.bashrc

CMD ["/bin/bash"]

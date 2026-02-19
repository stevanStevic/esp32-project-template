FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTAINER_USER=ubuntu
ARG IDF_VERSION=v5.5.2
ARG EIM_VERSION=v0.7.1

# ## System dependencies ##############################################
# Includes prerequisites for ESP-IDF (git, python3, cmake, ninja, etc.),
# Bluetooth/WiFi stack, and E2E testing tools.
RUN apt-get update \
    && apt-get install -y -q \
    sudo \
    vim \
    cmake \
    ninja-build \
    git \
    bash-completion \
    unzip \
    curl \
    wget \
    xz-utils \
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
    # For E2E testing with automatic WiFi connection management
    network-manager \
    && rm -rf /var/lib/apt/lists/*

# ## User setup ########################################################
RUN id -u "${CONTAINER_USER}" > /dev/null 2>&1 || useradd -m -s /bin/bash "${CONTAINER_USER}"
RUN echo "${CONTAINER_USER} ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/"${CONTAINER_USER}"
RUN chmod 0440 /etc/sudoers.d/"${CONTAINER_USER}"

# ## Install ESP-IDF via EIM CLI #######################################
# EIM installs:
#   - ESP-IDF source into  -p <path>/<version>/esp-idf  → /opt/esp/v5.5.2/esp-idf
#   - Tools & venv into    ~/.espressif/tools/           → /root/.espressif/tools/
#
# After installation we:
#   1. Move tools from /root/.espressif/tools → /opt/esp/tools  (accessible to all users)
#   2. Patch all path references (activation script + eim_idf.json)
#   3. Copy eim_idf.json to $HOME/.espressif/tools/ for the ubuntu user
#      (the VS Code extension hardcodes that lookup path)
#   4. Keep the EIM binary so the extension's eimPath check passes
RUN curl -fsSL "https://github.com/espressif/idf-im-ui/releases/download/${EIM_VERSION}/eim-cli-linux-x64.zip" \
        -o /tmp/eim-cli.zip \
    && unzip /tmp/eim-cli.zip -d /tmp/eim \
    && find /tmp/eim -name "eim" -type f -exec cp {} /usr/local/bin/eim \; \
    && chmod +x /usr/local/bin/eim \
    && eim install \
        -p /opt/esp \
        -i "${IDF_VERSION}" \
        -t esp32s3 \
        --skip-prerequisites-check true \
    && rm -rf /tmp/eim-cli.zip /tmp/eim

# ## Relocate tools from /root/.espressif to /opt/esp ##################
# Move the tools directory so it is accessible to non-root users.
# Then patch every reference from the old location to the new one.
RUN mv /root/.espressif/tools /opt/esp/tools \
    && rmdir /root/.espressif 2>/dev/null || true \
    && sed -i "s|/root/.espressif/tools|/opt/esp/tools|g" \
        /opt/esp/tools/activate_idf_${IDF_VERSION}.sh \
        /opt/esp/tools/eim_idf.json \
    && sed -i "s|/root/.espressif/tools|/opt/esp/tools|g" \
        /opt/esp/tools/python/${IDF_VERSION}/venv/pyvenv.cfg \
    && find /opt/esp/tools/python/${IDF_VERSION}/venv/bin -type f \
        -exec sed -i "s|/root/.espressif/tools|/opt/esp/tools|g" {} +

# Create convenience symlinks:
#   /opt/esp/idf          →  ESP-IDF source
#   /opt/esp/python       →  Python venv used by ESP-IDF
#   /opt/esp/activate.sh  →  EIM activation script
RUN ln -sf /opt/esp/${IDF_VERSION}/esp-idf /opt/esp/idf \
    && ln -sf /opt/esp/tools/python/${IDF_VERSION}/venv /opt/esp/python \
    && ln -sf /opt/esp/tools/activate_idf_${IDF_VERSION}.sh /opt/esp/activate.sh

# Create wrapper scripts in /usr/local/bin so idf.py (and friends) are on
# PATH for every context: interactive shells, scripts, and subprocess calls.
# The activation script defines them as aliases, which don't work outside
# interactive bash.  Real wrapper scripts solve that.
RUN printf '#!/bin/sh\nexec /opt/esp/tools/python/%s/venv/bin/python3 /opt/esp/%s/esp-idf/tools/idf.py "$@"\n' \
        "${IDF_VERSION}" "${IDF_VERSION}" > /usr/local/bin/idf.py \
    && printf '#!/bin/sh\nexec /opt/esp/tools/python/%s/venv/bin/python3 /opt/esp/%s/esp-idf/components/esptool_py/esptool/esptool.py "$@"\n' \
        "${IDF_VERSION}" "${IDF_VERSION}" > /usr/local/bin/esptool.py \
    && chmod +x /usr/local/bin/idf.py /usr/local/bin/esptool.py

# Make everything under /opt/esp readable/executable by all users
RUN chmod -R a+rX /opt/esp

# Mark ESP-IDF repo as safe so git describe works when run as non-root user.
# Without this, idf.py build reports "HEAD-HASH-NOTFOUND" as the IDF version.
RUN git config --system --add safe.directory /opt/esp/${IDF_VERSION}/esp-idf

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

# ## Install AWS CLI v2 ################################################
RUN curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o "awscliv2.zip" \
    && unzip awscliv2.zip \
    && ./aws/install \
    && rm -rf awscliv2.zip aws

# ## QEMU for ESP32 emulation #########################################
ENV QEMU_REL=esp_develop_9.0.0_20240606
ENV QEMU_SHA256=071d117c44a6e9a1bc8664ab63b592d3e17ceb779119dcb46c59571a4a7a88c9
ENV QEMU_DIST=qemu-xtensa-softmmu-${QEMU_REL}-x86_64-linux-gnu.tar.xz
ENV QEMU_URL=https://github.com/espressif/qemu/releases/download/esp-develop-9.0.0-20240606/${QEMU_DIST}

RUN wget --no-verbose ${QEMU_URL} \
    && echo "${QEMU_SHA256} *${QEMU_DIST}" | sha256sum --check --strict - \
    && tar -xf $QEMU_DIST -C /opt \
    && rm ${QEMU_DIST}

ENV PATH=/opt/qemu/bin:${PATH}

# ## Install extra Python dependencies into EIM's venv #################
# Use ESP-IDF's constraint file to prevent overwriting pinned dependencies
# (e.g. esptool) with incompatible versions pulled in by other packages.
COPY requirements.txt /tmp
RUN . /opt/esp/python/bin/activate \
    && pip install --upgrade pip \
    && pip install -c /opt/esp/tools/espidf.constraints.v5.5.txt -r /tmp/requirements.txt \
    && rm /tmp/requirements.txt

# ## Switch to dev user ################################################
USER ${CONTAINER_USER}
ENV USER=${CONTAINER_USER}
WORKDIR /home/${CONTAINER_USER}

# Place eim_idf.json where the VS Code extension expects it:
#   $HOME/.espressif/tools/eim_idf.json
# The extension hardcodes: path.join(process.env.HOME, ".espressif", "tools", "eim_idf.json")
RUN mkdir -p /home/${CONTAINER_USER}/.espressif/tools \
    && cp /opt/esp/tools/eim_idf.json /home/${CONTAINER_USER}/.espressif/tools/eim_idf.json

# Source the EIM-generated activation script on every shell login
RUN echo "source /opt/esp/activate.sh > /dev/null 2>&1" >> /home/${CONTAINER_USER}/.bashrc

CMD ["/bin/bash"]

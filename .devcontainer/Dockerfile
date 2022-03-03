FROM espressif/idf:v5.4.1

ARG DEBIAN_FRONTEND=nointeractive
ARG CONTAINER_USER=ubuntu

RUN apt-get update \
  && apt install -y -q \
  sudo \
  vim \
  cmake \
  git \
  libglib2.0-0 \
  libnuma1 \
  libpixman-1-0 \
  bluez \
  bluez-tools \
  dbus \
  udev \
  && rm -rf /var/lib/apt/lists/*

# Grant sudo privileges to the existing user
RUN id -u "${CONTAINER_USER}" > /dev/null 2>&1 || useradd -m -s /bin/bash "${CONTAINER_USER}"
RUN echo "${CONTAINER_USER} ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/"${CONTAINER_USER}"
RUN chmod 0440 /etc/sudoers.d/"${CONTAINER_USER}"

# QEMU
ENV QEMU_REL=esp_develop_9.0.0_20240606
ENV QEMU_SHA256=071d117c44a6e9a1bc8664ab63b592d3e17ceb779119dcb46c59571a4a7a88c9
ENV QEMU_DIST=qemu-xtensa-softmmu-${QEMU_REL}-x86_64-linux-gnu.tar.xz
ENV QEMU_URL=https://github.com/espressif/qemu/releases/download/esp-develop-9.0.0-20240606/${QEMU_DIST}

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN wget --no-verbose ${QEMU_URL} \
  && echo "${QEMU_SHA256} *${QEMU_DIST}" | sha256sum --check --strict - \
  && tar -xf $QEMU_DIST -C /opt \
  && rm ${QEMU_DIST}

ENV PATH=/opt/qemu/bin:${PATH}

# Install Python dependencies in the existing virtual environment
COPY requirements.txt /tmp
RUN VENV_PATH=$(find /opt/esp/python_env/ -type d -name "idf*_py*" | sort | tail -n 1) \
  && . $VENV_PATH/bin/activate \
  && pip install --upgrade pip \
  && pip install -r /tmp/requirements.txt \
  && rm /tmp/requirements.txt

# Ensure the virtual environment is activated by default
ENV PATH="$(find /opt/esp/python_env/ -type d -name "idf*_py*" | sort | tail -n 1)/bin:$PATH"

# Switch to the newly created user
USER ${CONTAINER_USER}
ENV USER=${CONTAINER_USER}
WORKDIR /home/${CONTAINER_USER}

# Source ESP-IDF setup automatically
RUN echo "source /opt/esp/idf/export.sh > /dev/null 2>&1" >> /home/${CONTAINER_USER}/.bashrc

ENTRYPOINT [ "/opt/esp/entrypoint.sh" ]

CMD ["/bin/bash", "-c"]

# Project

This is a template for ESP32 based projects.

# Development Environment Setup

**Note**: We recommend using a Linux host for development. One key reason is that attaching BLE devices to the
Docker container is fully supported in Linux, but it is not possible in Windows when using WSL.This is needed
for testing BLE provisioning with Python script.

## Common Setup

1. **Install Docker**:

    - **Linux**: Follow the
      [official Docker installation guide for Linux](https://docs.docker.com/engine/install/).
    - **Windows**: Install Docker for Windows Desktop from
      [here](https://hub.docker.com/editions/community/docker-ce-desktop-windows/).

2. **Install VS Code**:

    - Download and install VS Code from [here](https://code.visualstudio.com/).
    - Install the following extensions:
        - "Dev Containers"
        - (Optional but recommended) "Remote - WSL" (for Windows users)

3. **Set up SSH Key**:

    - **Generate an SSH Key**: Follow
      [this guide](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent)
      to generate a new SSH key and add it to your SSH agent.
    - **Add SSH Key to GitHub**: Follow the instructions in the same guide to add your SSH key to GitHub.

4. **Configure AWS Credentials (optional)**

    - Install AWS CLI (If Not Installed)
        ```bash
        sudo apt update && sudo apt install -y awscli
        ```
    - Configure AWS CLI Credentials

        ```bash
        aws configure
        ```

        You will be prompted to enter:

        - AWS Access Key ID (e.g., AKIAEXAMPLEKEY)
        - AWS Secret Access Key (e.g., wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY)
        - Default Region (e.g., us-east-1, us-west-2)
        - Default Output Format (e.g., json, table, text)

        This creates two files:

        ```bash
        ~/.aws/credentials (Stores AWS access and secret keys)
        ~/.aws/config (Stores default region and output format)
        ```

    - Verify Configuration

        ```bash
        aws sts get-caller-identity
        ```

        This should return information about your AWS account.

    - Add following line in `.devcontainer.json` in `mounts`
      `"source=${localEnv:HOME}/.aws,target=/home/ubuntu/.aws,type=bind", // Mount AWS credentials`

5. **Clone the Repository**:

    - Clone the repository:
        ```bash
        git clone --recurse-submodules git@github.com:your-github/project.git
        ```
    - Ensure you clone the repository in a location with good performance, preferably not in a Windows
      filesystem if using WSL.

## Preferred Development Environment: Linux Host

1. **Configure Docker**:

    - Ensure Docker is installed and running on your Linux system.

2. **Add User to Docker Group**:

    - To run Docker commands without `sudo`, add your user to the Docker group:
        ```bash
        sudo usermod -aG docker $USER
        ```
    - Log out and back in for this change to take effect.

3. **Install socat**:

    - Install `socat` to forward the SSH agent to the container:
        ```bash
        sudo apt update && sudo apt install -y socat
        ```

4. **Open Repository in VS Code**:

-   Open the cloned folder in VS Code:
    ```bash
    code <repo>
    ```
-   You should be prompted to open the folder inside a development container. Click "Reopen in container".
-   If not prompted, use the button on the bottom left (><) and select "Reopen in container".
-   Your development container is starting. The first time, this will take quite a while because the image
    needs to be built from [Dockerfile](.devcontainer/Dockerfile). Click on "Show Log" to get a progress bar.

5. **Build the Project**:
    - Ensure all Git submodules are checked out:
        ```bash
        git submodule update --init --recursive
        ```
    - Build the project by selecting `"ESP-IDF: Build Project"` from the bottom of VS Code.

## Alternate Development Environment: Windows with WSL

1. **Install Ubuntu in WSL**:

    - Install Ubuntu 20.04 (or higher) in WSL 2:
      [Manual installation steps](https://docs.microsoft.com/en-us/windows/wsl/install-win10#manual-installation-steps).

2. **Configure Docker to Access WSL**:

    - Configure Docker to access your Ubuntu installation in the settings: (Settings > Resources > WSL
      integration).

3. **Configure User in WSL**:

    - Add your WSL user to the Docker group:
        ```bash
        sudo usermod -aG docker $USER
        ```

4. **Install socat in WSL**:

    - Install `socat`:
        ```bash
        sudo apt update && sudo apt install -y socat
        ```

5. **Copy SSH Keys to WSL (if needed)**:

    - If you generated your SSH key on Windows, copy it to your WSL instance:
        ```bash
        cp -r /mnt/c/Users/USERNAME/.ssh ~/.ssh
        chmod 600 ~/.ssh/id_ed25519
        eval `ssh-agent -s`
        ssh-add ~/.ssh/id_ed25519
        ```

6. **Set Up SSH Agent in WSL**:

    - Ensure SSH agent is correctly configured in WSL:
        ```bash
        if [ -z "$SSH_AUTH_SOCK" ]; then
           RUNNING_AGENT="`ps -ax | grep 'ssh-agent -s' | grep -v grep | wc -l | tr -d '[:space:]'`"
           if [ "$RUNNING_AGENT" = "0" ]; then
                ssh-agent -s &> $HOME/.ssh/ssh-agent
           fi
           eval `cat $HOME/.ssh/ssh-agent`
        fi
        ssh-add <path to ssh key>
        ```

7. **Open Repository in VS Code**:

    - Open the repository in VS Code using WSL:
        ```bash
        code <repo>
        ```
    - You should be prompted to open the folder inside a development container. Click "Reopen in container".
    - Your development container is starting. The first time, this will take quite a while because the image
      needs to be built from [Dockerfile](.devcontainer/Dockerfile). Click on "Show Log" to get a progress
      bar.
    - When you clone the repository into the Linux file system, the Docker container can use quite a lot of
      RAM (>10GB) which might affect your host system or even bring it to the ground, requiring reboots. On
      Windows, you can create a .wslconfig file in your user profile folder (i.e.: C:\Users<name>.wslconfig)
      to limit the RAM usage of the Docker container (Make sure that you enable the "Use the WSL 2 based
      engine" option in the Docker Desktop application). See this for an overview. The relevant options are
      memory and processors. Make sure that you have no whitespaces after the options in the wslconfig-file.
      On macOS, memory usage of Docker in Activity Monitor can seem larger than it really is. See this Stack
      Overflow question explaining the difference between virtual and real memory.

8. **Build the Project**:

    - Ensure all Git submodules are checked out:
        ```bash
        git submodule update --init --recursive
        ```
    - Build the project by selecting `"ESP-IDF: Build Project"` from the bottom of VS Code.

## Flash the Board

This section explains how to flash your binary application into the board. The process differs slightly
depending on whether you're using a Linux host or Windows with WSL.

### 1. Install Drivers for Serial Communication

#### **Windows only**:

-   [Download CP210X driver](https://www.silabs.com/documents/public/software/CP210x_Windows_Drivers.zip) and
    install it.
-   After installation, connect your USB to serial cable and note the new `COM<X>` port in `Device Manager`.

### 2. Set Up Telnet for Serial Port Access

#### **Windows only**:

Since we are developing in remote container we need to tunnel serial port access towards real physical Windows
ports e.g. COM6. Here we will use the TELNET protocol to access the device communication port to flash and
monitor:

-   Download the latest version of `esp-tools` from [here](https://github.com/espressif/esptool/releases).
-   Extract the release and navigate to the extracted directory.
-   In `PowerShell`, run:
    ```bash
    .\esp_rfc2217_server.exe -v -p 4000 COM<X>
    ```

#### **Linux**:

-   Typically, your device will be recognized as `/dev/ttyACM0` or `/dev/ttyUSB0`.
-   If other USB devices are connected, these device names might be assigned different numbers (e.g.,
    `/dev/ttyACM1`). Ensure you're using the correct device for flashing.

### 3. Flash Your Project

-   In VS Code, click on the flash button or select `"ESP-IDF: Flash your project"`.
-   Ensure you've selected the correct serial port:
    -   **Linux**: Use `/dev/ttyACM0`, `/dev/ttyUSB0`, or the appropriate device name.
    -   **Windows**: Use the `rfc2217://host.docker.internal:4000?ign_set_control` port to flash over RFC
        server.

For more detailed information, check out the
[official espressif documentation on remote ports](https://docs.espressif.com/projects/esptool/en/latest/esp8266/esptool/remote-serial-ports.html)
and [this useful blog post](https://www.hackster.io/leoribg/esp-idf-in-docker-dev-container-e8510f).

## NOTE

Development under MacOS and Windows with VSCode containers are not tested. Feel free to contribute.

# Release Process

Documentation outlines the step-by-step [process](docs/release_process.md) to release production-ready
firmware for the project using GitHub Actions CI/CD.

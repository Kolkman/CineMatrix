import subprocess
from pathlib import Path
Import("env", "projenv")


result = subprocess.run(
    ['cat','./firmwareVersion'], stdout=subprocess.PIPE)
version = result.stdout.decode('utf-8').strip()
print("Version " + str(version))
print("Current CLI targets", COMMAND_LINE_TARGETS)
print("Current Build targets", BUILD_TARGETS)

#print(env.Dump())


def post_program_action(source, target, env):
    print("Program has been built!")
    program_path = target[0].get_abspath()
    print("Program path", program_path)
    # Use case: sign a firmware, do any manipulations with ELF, etc
    # This is really specific to the ESP environemtn
    projectpath = Path(env["PROJECT_PACKAGES_DIR"])
    projectpathlength = len(projectpath.parents)
    mkdir_command = "mkdir -p CineMatrixFirmware-"+version
    cp_command = "cp .pio/build/esp32dev-prod/firmware.bin  $PROJECT_PACKAGES_DIR/framework-arduinoespressif32/tools/sdk/esp32/bin/bootloader_dio_40m.bin " +\
        "$PROJECT_PACKAGES_DIR/framework-arduinoespressif32/tools/partitions/boot_app0.bin  .pio/build/esp32dev-prod/partitions.bin CineMatrixFirmware-"+version+"/"
    tar_cmnd=   "tar -cvzf $PROJECT_DIR/CineMatrixFirmware-"+version+".tar.gz  ./CineMatrixFirmware-"+version

    env.Execute(mkdir_command)
    env.Execute(cp_command)
    env.Execute(tar_cmnd)


env.AddPostAction(".pio/build/esp32dev-prod/firmware.bin", post_program_action)

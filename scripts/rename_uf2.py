import os
Import("env")

def rename_firmware(source, target, env):
    build_dir = env.subst("$BUILD_DIR")
    version = env.GetProjectOption("firmware_version", "dev")
    # Adapte ici le nom du fichier généré
    src = os.path.join(build_dir, "firmware.uf2")
    if not os.path.exists(src):
        # Essaye avec le nom du fichier principal
        src_alt = os.path.join(build_dir, "showcontrol_refactor.ino.uf2")
        if os.path.exists(src_alt):
            src = src_alt
    dst = os.path.join(build_dir, f"showcontrol_firmware_{version}.uf2")
    if os.path.exists(src):
        try:
            os.rename(src, dst)
            print(f"Renamed {src} -> {dst}")
        except Exception as e:
            print(f"Could not rename UF2: {e}")
    else:
        print(f"UF2 file not found: {src}")

env.AddPostAction("buildprog", rename_firmware)
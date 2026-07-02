#!/usr/bin/env python3

import os, sys, subprocess

include_dir = 'include'
src_dir = 'src'
build_dir = 'build/'
cc = 'g++'

flags = ['-I' + include_dir, '-O3', '-fopenmp', '-lpthread', '-DNDEBUG']


# Arguments
# -------------------------
args = sys.argv
do_clean = False
do_build = True
do_run = False
verbose = False

if '--clean' in args:
    args.remove('--clean')
    do_clean = True
    do_build = False

if '--verbose' in args:
    args.remove('--verbose')
    verbose = True

if '--debug' in args:
    args.remove('--debug')
    flags.append('-g')
    do_clean = True

if '--run' in args:
    args.remove('--run')
    do_run = True
    if do_clean:
        print("\033[1;31mCannot clean and run at the same time.\033[0m")
        exit()

# Functions
# -------------------------

def clean_file(file):
    print("\033[1;31mCleaning:\033[0m", file)
    os.remove(file)

def find_cpp_files(dir):
    cpp_files = []
    files = os.listdir(dir)

    for file in files:
        if '.cpp' in file:
            cpp_files.append(dir + '/' + file)
        else:
            cpp_files += find_cpp_files(dir + '/' + file)
    return cpp_files

def run_cmd(cmd):
    print(cmd)
    os.system(cmd)

def get_header_deps(cpp_file):
    dep = subprocess.run(
        [cc, '-MM', cpp_file] + flags,
        capture_output=True,
        text=True
    )
    return [file for file in dep.stdout.split() if '.hpp' in file]

def should_compile(file, obj):

    if not os.path.exists(obj):
        return True

    cpp_mod_time = os.path.getmtime(file)
    obj_mod_time = os.path.getmtime(obj)

    if cpp_mod_time > obj_mod_time:
        return True
    
    for header in get_header_deps(file):
        if os.path.getmtime(header) > obj_mod_time:
            return True

    return False

def create_directory(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)


def hash_str(s):
    h = 0
    for c in s:
        h = (h * 31 + ord(c)) & 0xFFFFFFFF
    return h

def get_color(s):
    return (hash_str(s) % 231) + 1

def get_clean_target_name(target):
    begin = len('src/apps/')
    end = target.rindex('.')
    return target[begin:end]

def get_target_name(target):
    name = get_clean_target_name(target)
    return f'\033[38;5;{get_color(name)}m{name}\033[0m'

# Build Project
# -------------------------
create_directory(build_dir + '/obj')
create_directory(build_dir + '/bin')


# Figure out which main is the target
targets = []
if len(args) == 1:
    targets = find_cpp_files(src_dir + '/apps')
else:
    for target in args[1:]:
        path = f'{src_dir}/apps/{target}.cpp'
        if not os.path.exists(path):
            print(f"Target {target} does not exist in /apps!")
            exit()
        targets.append(path)

print("\033[1;34mTargets:\033[0m ", end='')
print(", ".join([get_target_name(t) for t in targets]))


for target in targets:
    if do_build:
        print(f"\033[1;34mBuilding \033[0, {get_target_name(target)}")

    # Get all necessary header files
    handled_header_files = []
    header_files = get_header_deps(target)

    while True:
        if len(header_files) == 0: break
        header_file = header_files.pop()
        if header_file in handled_header_files:
            continue
        
        handled_header_files.append(header_file)
        new_headers = get_header_deps(header_file)
        for header in new_headers:
            header_files.append(header)

    if verbose:
        print("Header files:", handled_header_files)

    # Get all cpp files
    cpp_files = [target]
    for header in handled_header_files:
        cpp_path = f'{src_dir}{header[7:-4]}.cpp'
        if os.path.exists(cpp_path):
            cpp_files.append(cpp_path)

    if verbose:
        print("Cpp files:", cpp_files)

    # Compile all cpp files (and collect o files)
    o_files = []
    compiled_something = False
    for cpp_file in cpp_files:
        o_file = f'build/obj/{cpp_file[4:-4]}.o'
        o_files.append(o_file)

        dir = o_file[:o_file.rfind('/')]
        create_directory(dir)

        if do_clean and os.path.exists(o_file):
            clean_file(o_file)

        if not should_compile(cpp_file, o_file):
            continue

        if do_build:
            compiled_something = True
            run_cmd(
                f'{cc} -c {cpp_file} -o {o_file} {' '.join(flags)}'
            )

    if verbose:
        print("Object files:", o_files)

    # Compile main with o files
    exe_path = f'build/bin/{get_clean_target_name(target)}'
    if do_build and compiled_something:
        run_cmd(f'{cc} {' '.join(o_files)} -o {exe_path} {' '.join(flags)}')
    if do_clean and os.path.exists(exe_path):
        clean_file(exe_path)

if do_build:
    print("\033[1;32mBuild finished.\033[0m")

if do_run:
    for target in targets:
        name = get_clean_target_name(target)
        colored_name = get_target_name(target)
        exe_path = f'build/bin/{name}'
        print(f"\033[1;34mRunning\033[0m {colored_name}:")
        subprocess.run(exe_path)
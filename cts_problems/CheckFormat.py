import re
import os
import argparse

max_file_size = 100 * 1024 * 1024 # byte
max_components_num = 250000
max_net_num = 100000

constraints_pattern = re.compile(r'''
net_unit_r\s*=\s*([+-]?\d+(\.\d+)?([eE][+-]?\d+)?)\s*
net_unit_c\s*=\s*([+-]?\d+(\.\d+)?([eE][+-]?\d+)?)\s*
max_net_rc\s*=\s*([+-]?\d+(\.\d+)?([eE][+-]?\d+)?)\s*
max_fanout\s*=\s*(\d+)\s*
buffer_delay\s*=\s*(\d+)\s*
''', re.X)


problem_pattern = re.compile(r"""
    UNITS\s+DISTANCE\s+MICRONS\s+(\d+)\s*;.*?                    # 提取unit_distance
    DIEAREA\s+\(\s*(\d+)\s+(\d+)\s*\)\s+\(\s*\d+\s+\d+\s*\)\s+\(\s*(\d+)\s+(\d+)\s*\).*?   # 提取x_min, y_min, x_max, y_max
    FF\s+\(\s*(\d+)\s+(\d+)\s*\)\s*;.*?                          # 提取ff_w, ff_h
    BUF\s+\(\s*(\d+)\s+(\d+)\s*\)\s*;.*?                         # 提取buf_w, buf_h
    CLK\s+\(\s*(\d+)\s+(\d+)\s*\)\s*;.*?                         # 提取clk_x, clk_y
    COMPONENTS\s+(\d+)\s*;\n(.*?)END\s+COMPONENTS\s*;.*?         # 提取ff_num, components             
    """, re.S | re.X)

solution_pattern = re.compile(r"""
    UNITS\s+DISTANCE\s+MICRONS\s+(\d+)\s*;.*?                    # 提取unit_distance
    DIEAREA\s+\(\s*(\d+)\s+(\d+)\s*\)\s+\(\s*\d+\s+\d+\s*\)\s+\(\s*(\d+)\s+(\d+)\s*\).*?   # 提取x_min, y_min, x_max, y_max
    FF\s+\(\s*(\d+)\s+(\d+)\s*\)\s*;.*?                          # 提取ff_w, ff_h
    BUF\s+\(\s*(\d+)\s+(\d+)\s*\)\s*;.*?                         # 提取buf_w, buf_h
    CLK\s+\(\s*(\d+)\s+(\d+)\s*\)\s*;.*?                         # 提取clk_x, clk_y
    COMPONENTS\s+(\d+)\s*;\n(.*?)END\s+COMPONENTS\s*;.*?         # 提取componet_num, components_content             
    NETS\s+(\d+)\s*;\n(.*?)END\s+NETS\s*;.*?                     # 提取net_num, net_content             
    """, re.S | re.X)

ff_pattern = re.compile(r"""
    -\s+(\w+)\s+FF\s+\(\s*(\d+)\s+(\d+)\s*\)\s*; # 提取ff name、x、y
    """, re.S | re.X)

buf_pattern = re.compile(r"""
    -\s+(\w+)\s+BUF\s+\(\s*(\d+)\s+(\d+)\s*\)\s*; # 提取buf name、x、y
    """, re.S | re.X)

net_pattern = re.compile(r"""
    -\s+(\w+)\s+\(\s*(\w+)\s*\)\s+\(([\w\s]+)\)\s*;  # 提取net_name、driver_name和sink_names
    """, re.S | re.X)


def print_and_exit(msg):
    if len(msg) > 100:
        print(msg[:100] + "...")
    else:
        print(msg)
    exit(1)

def check_file_exist(base_dir):
    constraints_path = os.path.join(base_dir, "constraints.txt")
    problem_path = os.path.join(base_dir, "problem.def")
    solution_path = os.path.join(base_dir, "solution.def")
    if (not os.path.exists(constraints_path) or
    not os.path.exists(problem_path) or
    not os.path.exists(solution_path)):
        return False, "ERROR_1: file does not exist", None

    file_size = os.path.getsize(solution_path) # byte
    if file_size > 100 * 1024 * 1024:
        return False, "ERROR_2: solution.def size exceeds 10MB", None
    return True, "Check File Exist PASS", (constraints_path, problem_path, solution_path)

def check_constraints_format(constraints_path):
    with open(constraints_path, 'r') as file:
        content = file.read()
        if not constraints_pattern.match(content):
            return False, f"ERROR_3: constraints.txt does not match the required format"
    return True, "Check Constraint Format PASS"

def check_problem_format(content):
    match = problem_pattern.search(content)
    if match is None:
        return False, f"ERROR_4: problem.def format incorrect", None
    x_min, y_min, x_max, y_max = map(int, [match.group(2), match.group(3), match.group(4), match.group(5)])
    clk_x, clk_y = int(match.group(10)), int(match.group(11))
    if clk_x < x_min or clk_x > x_max or clk_y < y_min or clk_y > y_max:
        return False, f"ERROR_5: CLK ({clk_x}, {clk_y}) out of range", None
    ff_num = int(match.group(12))
    components = match.group(13)
    ff_matches = ff_pattern.findall(components)
    if ff_matches is None:
        return False, f"ERROR_6: ff_matches is empty"
    ffs_names = set()
    for name, x, y in ff_matches:
        x, y = int(x), int(y)
        if not name.startswith("FF"):
            return False, f"ERROR_7: FF component name: {name} does not start with 'FF'", None
        if x < x_min or x > x_max or y < y_min or y > y_max:
            return False, f"ERROR_8: FF {name} ({x}, {y}) out of range", None
        ffs_names.add(name)
    if ff_num != len(ffs_names):
        return False, f"ERROR_9: FF component numbers mismatch, {ff_num} vs {len(ffs_names)}", None
    return True, "Check Problem Format PASS", (x_min, y_min, x_max, y_max, ffs_names)

def check_solution_change(problem_content, solution_content):
    problem_lines = problem_content.split("\n")
    solution_lines = solution_content.split("\n")
    for line_number, line in enumerate(problem_lines, 1):
        if line_number >= len(solution_lines):
            return False, f"ERROR_10: Solution line {line_number} does not match the required format"
        if (line.startswith("UNITS ") or
            line.startswith("DIEAREA ") or
            line.startswith("FF ") or
            line.startswith("BUF ") or
            line.startswith("CLK ") or
            line.startswith("- FF")
        ):
            if line != solution_lines[line_number-1]:
                return False, f"ERROR_11: problem content and solution content mismatch, line {line_number}, {line} vs {solution_lines[line_number-1]}"
        else:
            continue
    return True, "Check Solution Change PASS"


def check_solution_format(solution_content, x_min, y_min, x_max, y_max, ffs_names):
    match = solution_pattern.search(solution_content)
    if match is None:
        return False, f"ERROR_12: solution.def format incorrect"
    components_num = int(match.group(12))
    if components_num > max_components_num:
        return False, f"ERROR_13: components_num ({components_num}) exceeds max_components_num ({max_components_num})"
    components_content = match.group(13)
    nets_num = int(match.group(14))
    if nets_num > max_net_num:
        return False, f"ERROR_14: nets_num ({nets_num}) exceeds max_net_num ({max_net_num})"
    nets_content = match.group(15)

    buf_matches = buf_pattern.findall(components_content)
    bufs_names = set()
    for name, x, y in buf_matches:
        if not name.startswith("BUF"):
            return False, f"ERROR_15: BUF component name: {name} does not start with 'BUF'"
        # x, y = int(x), int(y)
        # if x < x_min or x > x_max or y < y_min or y > y_max:
        #     return False, f"ERROR_16: BUF {name} ({x}, {y}) out of range"
        bufs_names.add(name)

    if components_num != len(ffs_names) + len(bufs_names):
        return False, f"ERROR_17: COMPONENTS number mismatch: {components_num} vs {len(ffs_names) + len(bufs_names)}"

    components = ffs_names | bufs_names | {"CLK"}
    net_matches = net_pattern.findall(nets_content)
    if net_matches is None or len(net_matches) == 0:
        return False, f"ERROR_18: nets_content is empty"
    net_driver_to_net_name = {}
    net_name_to_net_driver = {}
    component_to_driver = {"CLK": None}

    for net_name, driver_name, sinks in net_matches:
        if not net_name.startswith("net_"):
            return False, f"ERROR_19: net_name ({net_name}) does not start with 'net_'"
        if driver_name not in components:
            return False, f"ERROR_20: net({net_name})'s unknown driver's name: {driver_name}"

        # net_name and driver_name should be corresponded
        if driver_name in net_driver_to_net_name and net_driver_to_net_name[driver_name] != net_name:
            return False, f"ERROR_21: net_driver to different net_name : {driver_name} -> {net_name}/{net_driver_to_net_name[driver_name]}"
        if net_name in net_name_to_net_driver and net_name_to_net_driver[net_name] != driver_name:
            return False, f"ERROR_22: net_name to different net_driver : {net_name} -> {driver_name}/{net_name_to_net_driver[net_name]}"
        net_driver_to_net_name[driver_name] = net_name
        net_name_to_net_driver[net_name] = driver_name

        sink_name_lst = [name.strip() for name in sinks.split()]
        for sink_name in sink_name_lst:
            if sink_name not in components:
                return False, f"ERROR_23: unknown sink's name: {sink_name}"
            if sink_name == "CLK":
                return False, f"ERROR_23: sink's name: {sink_name} == CLK"
            if sink_name in component_to_driver and component_to_driver[sink_name] != "CLK":
                if component_to_driver[sink_name] != driver_name:
                    return False, f"ERROR_24: net({net_name})'s sink {sink_name} has ambiguous driver: {component_to_driver[sink_name]}"
                else:
                    return False, f"ERROR_25: net({net_name}), driver ({driver_name}) sink {sink_name} multiply defined"
            component_to_driver[sink_name] = driver_name

    if len(net_driver_to_net_name) != len(net_name_to_net_driver):
        return False, f"ERROR_26: net_driver and net_name should be the same {len(net_driver_to_net_name)} vs {len(net_name_to_net_driver)}"

    if len(component_to_driver) != len(ffs_names) + len(bufs_names) + 1:
        dangling_components = (ffs_names | bufs_names | {"CLK"}) - set(component_to_driver.keys())
        return False, f"ERROR_27: found dangling components: {len(dangling_components)}, {dangling_components}"

    no_driver_components = (ffs_names | bufs_names | {"CLK"}) - set(component_to_driver.keys())
    if len(no_driver_components) != 0:
        return False, f"ERROR_28: {no_driver_components} these components have no driver"

    connected_ffs = set()
    for ff_name in ffs_names:
        pre_name = component_to_driver[ff_name]
        visited = set()
        while pre_name is not None:
            if pre_name in visited:
                return False, f"ERROR_29: detected loop for {ff_name} and {pre_name}"
            if pre_name.startswith("FF"):
                return False, f"ERROR_30: FF({ff_name}) connected to another FF({pre_name})"
            if pre_name == "CLK":
                connected_ffs.add(ff_name)
                break
            visited.add(pre_name)
            pre_name = component_to_driver[pre_name]
    if len(connected_ffs) != len(ffs_names):
        return False, f"ERROR_31: {ffs_names-connected_ffs} not connected to CLK"

    return True, "check Solution Format PASS"

def check_format(directory):
    status, msg, params = check_file_exist(directory)
    print_and_exit(msg) if not status else print(msg)

    constraints_path, problem_path, solution_path = params
    status, msg = check_constraints_format(constraints_path)
    print_and_exit(msg) if not status else print(msg)

    with open(problem_path, 'r') as file:
        problem_content = file.read()
    status, msg, params = check_problem_format(problem_content)
    print_and_exit(msg) if not status else print(msg)
    x_min, y_min, x_max, y_max, ffs_names = params

    with open(solution_path, 'r') as file:
        solution_content = file.read()
    status, msg = check_solution_change(problem_content, solution_content)
    print_and_exit(msg) if not status else print(msg)

    status, msg = check_solution_format(solution_content, x_min, y_min, x_max, y_max, ffs_names)
    print_and_exit(msg) if not status else print(msg)



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("directory", type=str, default="example_problem",
                        help="specify the directory of problem files")
    args = parser.parse_args()
    check_format(args.directory)




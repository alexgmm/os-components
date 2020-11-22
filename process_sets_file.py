import re

with open("src/instance/instance_sets") as sets_file:
    num_sets = int(sets_file.readline())
    
    for i in range(num_sets):
        num_instances = int(sets_file.readline())
        set_name = sets_file.readline().split('\n')[0]
        set_name = re.split('\s+',set_name)[0]

        for j in range(num_instances):
            line = re.split("\s+",sets_file.readline())

            inst_name, best_known = re.split('\s+',line[0])[0], line[3]
            
            path = "inst/" + set_name + "/" + inst_name

            with open(path, "a") as inst_file:
                inst_file.write(best_known)
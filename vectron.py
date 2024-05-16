import sys
import os

args = sys.argv
seq_path = sys.argv[1]
vectron_path = sys.argv[2]
file_name = sys.argv[3]

with open("script_name.txt", 'w') as s:
    s.write(sys.argv[3])
s.close()

command = "codon build " + file_name + " -plugin " + seq_path + " -plugin " + vectron_path + " -release"

# Execute the command
exit_status = os.system(command)

if exit_status == 0:
    print("Command executed successfully")
else:
    print("Command failed with exit status:", exit_status)
    print("Usage: ./vectron <seq_path> <vectron_path> <file_path>")
 
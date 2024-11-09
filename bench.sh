echo "Non-vectorized runs:"
echo "--------------------"
find ./applications/v2.0/*.codon | awk -v params="${*:1}" '{system("./run.sh -release "$0" --jit --no-vec "params)}' 2>&1 | grep -i "checksum\|took"
echo
echo "Vectorized runs (v1.0):"
echo "--------------------"
find ./applications/v1.0/*.codon | awk -v params="${*:1}" '{system("./run.sh -release "$0" --jit "params)}' 2>&1 | grep -i "checksum\|took"
echo
echo "Vectorized runs (v2.0):"
echo "--------------------"
find ./applications/v2.0/*.codon | awk -v params="${*:1}" '{system("./run.sh -release "$0" --jit "params)}' 2>&1 | grep -i "checksum\|took"
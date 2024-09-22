#!/usr/bin/env bash

if [ ! -d "${VECTRON_CODON_PATH}" ]
then
    echo "Codon not installed at ${SEQURE_CODON_PATH}" >&2
    return
fi

echo "Codon path: $VECTRON_CODON_PATH"

if [[ $* == */v2.0/* ]]
then
    DISABLE_OPTS='vectron-byPass-analysis vectron-loop-analysis vectron-ListInitializer vectron-VarTypeSelector vectron-funcreplacement'
else
    DISABLE_OPTS=ternary-vec
fi
OPTS_FLAG=`echo $DISABLE_OPTS | awk '{for(i=1; i<=NF; i++) printf("--disable-opt=%s ", $i)}'`

if [[ $* == *--jit* ]]
then
    echo "Running $2 in $1 mode ..."
    /usr/bin/time -v $VECTRON_CODON_PATH/bin/codon run $OPTS_FLAG -plugin vectron ${*:1}
else
    if [[ $* == *--build* ]]
    then
        rm -f ./vectronx
        echo "Compiling $2 in $1 mode ..."
        CC=clang CXX=clang++ $VECTRON_CODON_PATH/bin/codon build $OPTS_FLAG -plugin vectron $1 -o vectronx ${*:2}
    fi

    if [ ! -f "./vectronx" ]
    then
        echo "Vectron is not built. Make sure to add --build flag in first run or use --jit mode." >&2
        return
    fi

    if [[ ! $* == *--build-only* ]]
    then
        echo "Running $2 in $1 mode ..."
        /usr/bin/time -v ./vectronx ${*:2}
    fi
fi

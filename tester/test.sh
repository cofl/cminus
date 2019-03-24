TESTDIR=$(realpath $(dirname ${BASH_SOURCE[0]}))
ROOT=$(realpath "$TESTDIR/..")
MTEST="$TESTDIR/test"
CTEST="$TESTDIR/real"
CMC="$ROOT/cmc"
cd $ROOT
make -s cmc

for f in $ROOT/input/*.cm
do
    if [ "$(basename $f)" == "22.guess.cm" ]; then continue; fi
    if [ "$(basename $f)" == "23.asm.cm" ]; then continue; fi
    if [ "$(basename $f)" == "13.hack_array.cm" ]; then continue; fi
    rm -f $MTEST $CTEST
    echo "Testing file $f"
    $CMC $f -o - | gcc -x assembler - -o $MTEST
    cat "$TESTDIR/test.cpp" $f | gcc -x c++ - -o $CTEST
    INPUTDATA="$TESTDIR/$(basename $f)"
    if [ -f $INPUTDATA ]
    then
        DIFF=$(diff <(cat $INPUTDATA | $MTEST) <(cat $INPUTDATA | $CTEST))
    else
        DIFF=$(diff <($MTEST) <($CTEST))
    fi
    if [ ! -z "$DIFF" ]
    then
        echo "[FAIL]:"
        echo $DIFF
    else
        echo "[PASS]"
    fi
    echo
done
rm -f $MTEST $CTEST
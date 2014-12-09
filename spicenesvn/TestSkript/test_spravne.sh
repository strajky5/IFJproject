#/bin/bash

# IFJ09 - Basic tests
# Interpret name (path)
interpret="./main"


# Headline
echo "~~~~~~~~~~~~"
echo "IFJ09 tests:"
echo "~~~~~~~~~~~~"


# Test class
echo "-------------"
echo "1) Run tests:"
echo "-------------"

# Run tests
# Inter error value
RET=5
# Test label
echo "Test: No parameters"
printf "  Return value: "
$interpret 2>/dev/null 1>/dev/null
RETURNED=$?
if [ "x$RETURNED" == "x$RET" ]; then
    echo "[OK] ($RETURNED vs. $RET)"
else
    echo "[ERROR] ($RETURNED vs. $RET)"
fi
# Test label
echo "Test: To many parameters"
printf "  Return value: "
$interpret "parameter" "parameter" 2>/dev/null 1>/dev/null
RETURNED=$?
if [ "x$RETURNED" == "x$RET" ]; then
    echo "[OK] ($RETURNED vs. $RET)"
else
    echo "[ERROR] ($RETURNED vs. $RET)"
fi
# Test label
echo "Test: Bad File"
printf "  Return value: "
$interpret "probably_not_existing_source_file.wow" 2>/dev/null 1>/dev/null
RETURNED=$?
if [ "x$RETURNED" == "x$RET" ]; then
    echo "[OK] ($RETURNED vs. $RET)"
else
    echo "[ERROR] ($RETURNED vs. $RET)"
fi


# Test class
echo "-------------------"
echo "2) Black-box tests:"
echo "-------------------"
# Test class directory
spravne="./spravne/"

# Black-box tests
for src in `ls $spravne | grep -E "*.src"`; do
    # Test name
    TEST=`echo $src | sed 's/\.\(.*\)//g'`

    # Input and expected output
  #  IN=`echo $src | sed 's/\.src/\.in/g'`
   # OUT=`echo $src | sed 's/\.src/\.out/g'`

    # Expected return value
    if [ "x`echo $src | grep '.lex.src' | wc -l`" == "x1" ]; then
        RET=1
    elif [ "x`echo $src | grep '.syn.src' | wc -l`" == "x1" ]; then
        RET=2
    elif [ "x`echo $src | grep '.sem.src' | wc -l`" == "x1" ]; then
        RET=3
    elif [ "x`echo $src | grep '.int.src' | wc -l`" == "x1" ]; then
        RET=4
    else
        RET=0
    fi;


    #Test label
    echo "Test: $TEST"

    #Result comparison
#    if [ "x$RET" == "x0" ]; then
   #     echo "DIFF: "
        cat $spravne$IN | "$interpret" $spravne$src #| diff - $spravne$OUT
#    else
#        printf "  Return value: "
#        cat $spravne$IN | "$interpret" $spravne$src 2>/dev/null 1>/dev/null
#        RETURNED=$?
#        if [ "x$RETURNED" == "x$RET" ]; then
#            echo "[OK] ($RETURNED vs. $RET)"
#        else
#            echo "[ERROR] ($RETURNED vs. $RET)"
#        fi
#    fi
done;

exit 0


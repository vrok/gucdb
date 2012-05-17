

for i in `seq 1 70`; do ./utils/test_gen.py ./tests/input.$i ./tests/output.$i; done

for i in `seq 1 70`; do rm -rf /tmp/main.*; echo running $i; ./db < ./tests/input.$i > ./tests/act_output.$i; cmp ./tests/output.$i ./tests/act_output.$i; done

for i in `seq 1 70`; do rm -rf /tmp/main.*; echo running $i; ./db < ./tests/input.$i > ./tests/act_output.$i 2>/dev/null; cmp ./tests/output.$i ./tests/act_output.$i; done


 for i in `seq 1 40`; do ./test_slabs.py ./../db /tmp || (echo 'YEP!' ; exit) ; done

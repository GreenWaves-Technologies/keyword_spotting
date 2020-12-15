make test_accuracy SMALL=1  USE_HIGH_PREC=0 USE_POWER=1 > log_test_small_power.txt
make test_accuracy SMALL=1  USE_HIGH_PREC=1 USE_POWER=1 > log_test_small_hp_power.txt
make test_accuracy SMALL=1  USE_HIGH_PREC=0 USE_POWER=0 > log_test_small_spectr.txt
make test_accuracy SMALL=1  USE_HIGH_PREC=1 USE_POWER=0 > log_test_small_hp_spectr.txt

make test_accuracy MEDIUM=1 USE_HIGH_PREC=0 USE_POWER=1 > log_test_medium_power.txt
make test_accuracy MEDIUM=1 USE_HIGH_PREC=1 USE_POWER=1 > log_test_medium_hp_power.txt
make test_accuracy MEDIUM=1 USE_HIGH_PREC=0 USE_POWER=0 > log_test_medium_spectr.txt
make test_accuracy MEDIUM=1 USE_HIGH_PREC=1 USE_POWER=0 > log_test_medium_hp_spectr.txt

make test_accuracy LARGE=1  USE_HIGH_PREC=0 USE_POWER=1 > log_test_large_power.txt
make test_accuracy LARGE=1  USE_HIGH_PREC=1 USE_POWER=1 > log_test_large_hp_power.txt
make test_accuracy LARGE=1  USE_HIGH_PREC=0 USE_POWER=0 > log_test_large_spectr.txt
make test_accuracy LARGE=1  USE_HIGH_PREC=1 USE_POWER=0 > log_test_large_hp_spectr.txt
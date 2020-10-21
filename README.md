# Models

## Small

### Training
    python train_ahmad_q.py    --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20  --model_architecture ds_cnn_q --model_size_info 5 64 10 4 2 2 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 --quantize 2000 --summary_dir work_q_sym/DS_CNN_104020/retrain_log --train_dir work_q_sym/DS_CNN_104020/training/ --learning_rate 0.0005,0.0002,0.0001 --how_many_training_steps 2000,1000,1000 
    python freeze___ahmad_q.py --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20  --model_architecture ds_cnn_q --model_size_info 5 64 10 4 2 2 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 64 3 3 1 1 --quantize True --start_checkpoint work_q_sym/DS_CNN_104020/training/best/ds_cnn_8949.ckpt-5000 --output_file work_q_sym/DS_CNN_104020/ds_cnn_q_frozen_quant.pb
    tflite_convert --graph_def_file work_q_sym/DS_CNN_104020/ds_cnn_q_frozen_quant.pb --output_file work_q_sym/DS_CNN_104020/ds_cnn_q_frozen_quant.tflite --input_format=TENSORFLOW_GRAPHDEF --output_format=TFLITE --input_arrays=Reshape_1 --output_arrays=labels_softmax --inference_type=QUANTIZED_UINT8 --mean_values 128 --std_dev_values 0.51619

    TFLITE
    Test: Accuracy: 89.22%                                  Validation: Accuracy: 89.24%
    [[408   0   0   0   0   0   0   0   0   0   0   0]       [[371   0   0   0   0   0   0   0   0   0   0   0]
     [  2 321   1   9   6  13  17   6   8   1  10  14]        [  1 287   3   8   6  14  13  13  10   1   6   9]
     [  1  12 375   7   0   1  21   1   0   0   0   1]        [  1  10 360   9   0   6  10   0   0   0   0   1]
     [  0   7   3 353   1  22   4   1   0   0   1  13]        [  2  18   2 340   3  16   1   4   3   0   1  16]
     [  2  16   0   1 383   1   2   0   4   7   8   1]        [  0   4   0   0 319   1   5   2   1  10   6   2]
     [  3  20   0  11   0 353   4   0   3   0   3   9]        [  1   9   0  16   0 330   3   1   0   0   5  12]
     [  2   3   9   3   2   0 384   7   1   0   1   0]        [  0  10   8   2   1   1 322   5   0   0   3   0]
     [  1  14   1   0   1   1   7 369   0   0   1   1]        [  0  11   2   0   0   0   3 343   0   1   0   3]
     [  3  13   1   0   5   4   0   0 352  14   1   3]        [  2  13   0   0   5   2   0   0 332   8   1   0]
     [  2   8   0   0  28   1   4   2   8 335   5   9]        [  0   6   0   1  25   0   2   1  12 314   6   6]
     [  2   4   0   0  10   8   0   0   1   0 383   3]        [  3   6   0   0  10   2   0   0   1   3 322   3]
     [  3  12   0  19   1  11   2   0   0   4   4 346]]       [  1  13   0  11   1  11   3   1   3   1   1 326]]

### GAP

Generate samples:

	python utils/generate_samples_images.py --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20

Run inference:

	make clean all run platform=gvsoc SMALL=1

Performances:

                     S1_Conv2d_64x1x10x4_Relu: Cycles:     452888, Operations:     328000, Operations/Cycle: 0.724241
                      S2_Conv2d_64x1x3x3_Relu: Cycles:      25941, Operations:      80000, Operations/Cycle: 3.083921
                     S3_Conv2d_64x64x1x1_Relu: Cycles:      61355, Operations:     512000, Operations/Cycle: 8.344878
                      S4_Conv2d_64x1x3x3_Relu: Cycles:      25110, Operations:      80000, Operations/Cycle: 3.185982
                     S5_Conv2d_64x64x1x1_Relu: Cycles:      61014, Operations:     512000, Operations/Cycle: 8.391517
                      S6_Conv2d_64x1x3x3_Relu: Cycles:      24764, Operations:      80000, Operations/Cycle: 3.230496
                     S7_Conv2d_64x64x1x1_Relu: Cycles:      60839, Operations:     512000, Operations/Cycle: 8.415654
                      S8_Conv2d_64x1x3x3_Relu: Cycles:      24688, Operations:      80000, Operations/Cycle: 3.240441
                     S9_Conv2d_64x64x1x1_Relu: Cycles:      60851, Operations:     512000, Operations/Cycle: 8.413995
                         S10_AveragePool_25x5: Cycles:       6854, Operations:       8000, Operations/Cycle: 1.167202
                         S11_Linear_12x64x1x1: Cycles:       1997, Operations:        768, Operations/Cycle: 0.384577
                                  S12_SoftMax: Cycles:       1674, Operations:         12, Operations/Cycle: 0.007168

                                        Total: Cycles:     807975, Operations:    2704780, Operations/Cycle: 3.347604

## Medium


    python train_ahmad_q.py    --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20 --model_architecture ds_cnn_q --model_size_info 5 172 10 4 2 1 172 3 3 2 2 172 3 3 1 1 172 3 3 1 1 172 3 3 1 1 --quantize 5000 --learning_rate 0.0005,0.0001,0.00002 --how_many_training_steps 10000,10000,10000 --summaries_dir work_q_sym/DS_CNN_M_104020/retrain_logs --train_dir work_q_sym/DS_CNN_M_104020/training 
    python freeze___ahmad_q.py --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20 --model_architecture ds_cnn_q --model_size_info 5 172 10 4 2 1 172 3 3 2 2 172 3 3 1 1 172 3 3 1 1 172 3 3 1 1 --quantize True --start_checkpoint work_q_sym/DS_CNN_M_104020/training/best/ds_cnn_q_?????? --output_file work_q_sym/DS_CNN_M_104020/ds_cnn_q_frozen_quant.pb
    tflite_convert --graph_def_file work_q_sym/DS_CNN_M_104020/ds_cnn_q_frozen_quant.pb --output_file work_q_sym/DS_CNN_M_104020/ds_cnn_q_frozen_quant.tflite --input_format=TENSORFLOW_GRAPHDEF --output_format=TFLITE --input_arrays=Reshape_1 --output_arrays=labels_softmax --inference_type=QUANTIZED_UINT8 --mean_values 128 --std_dev_values 0.51619

    Test: Accuracy: 92.69%                                  Validation: Accuracy: 92.7%
    [[408   0   0   0   0   0   0   0   0   0   0   0]     [[371   0   0   0   0   0   0   0   0   0   0   0]
     [  0 355   3   7   6   6   8   4   4   0   7   8]      [  1 326   4   4   2   5   4   5   3   2   6   9]  
     [  0  10 401   0   0   0   8   0   0   0   0   0]      [  0   3 381   2   1   0   9   0   0   0   0   1]  
     [  0   3   0 382   0  12   3   0   0   0   0   5]      [  1   9   3 378   1   5   1   0   0   0   1   7]  
     [  0  14   0   0 392   1   1   0   2   5  10   0]      [  0   2   1   0 321   2   3   0   0  10   9   2]  
     [  0   7   2  15   1 367   1   0   2   0   1  10]      [  1   3   1  11   0 351   1   0   0   0   1   8]  
     [  1   5   3   1   0   0 395   4   0   1   2   0]      [  0   2  11   1   1   0 332   2   0   0   1   2]  
     [  0  17   0   0   1   1   4 370   1   0   1   1]      [  0   8   1   0   0   1   4 346   0   0   1   2]  
     [  0  11   0   0   8   6   0   0 359   6   2   4]      [  2   8   0   1   4   1   0   1 338   8   0   0]  
     [  0   9   0   1  25   0   1   0   5 354   2   5]      [  1   2   0   1  22   1   2   0   7 325   9   3]  
     [  0   5   0   0   3   3   1   0   0   0 394   5]      [  2   2   0   0  11   0   1   1   1   0 330   2]  
     [  0   8   1  22   2   9   1   0   0   0   4 355]]     [  0   7   1  19   2  14   0   1   0   3   4 321]]   

### GAP

Generate samples:

	python utils/generate_samples_images.py --dct_coefficient_count 10 --window_size_ms 40 --window_stride_ms 20

Run inference:

	make clean all run platform=gvsoc MEDIUM=1

Performances:

	                S1_Conv2d_172x1x10x4_Relu: Cycles:    2272137, Operations:    1763000, Operations/Cycle: 0.775922
                     S2_Conv2d_172x1x3x3_Relu: Cycles:      40641, Operations:     111800, Operations/Cycle: 2.750916
                   S3_Conv2d_172x172x1x1_Relu: Cycles:     183783, Operations:    1922960, Operations/Cycle: 10.463209
                     S4_Conv2d_172x1x3x3_Relu: Cycles:      40043, Operations:     111800, Operations/Cycle: 2.791999
                   S5_Conv2d_172x172x1x1_Relu: Cycles:     183693, Operations:    1922960, Operations/Cycle: 10.468336
                     S6_Conv2d_172x1x3x3_Relu: Cycles:      39190, Operations:     111800, Operations/Cycle: 2.852769
                   S7_Conv2d_172x172x1x1_Relu: Cycles:     183416, Operations:    1922960, Operations/Cycle: 10.484145
                     S8_Conv2d_172x1x3x3_Relu: Cycles:      38997, Operations:     111800, Operations/Cycle: 2.866887
                   S9_Conv2d_172x172x1x1_Relu: Cycles:     183318, Operations:    1922960, Operations/Cycle: 10.489750
                         S10_AveragePool_13x5: Cycles:      10874, Operations:      11180, Operations/Cycle: 1.028141
                        S11_Linear_12x172x1x1: Cycles:       2327, Operations:       2064, Operations/Cycle: 0.886979
                                  S12_SoftMax: Cycles:       1725, Operations:         12, Operations/Cycle: 0.006957

                                        Total: Cycles:    3180144, Operations:    9915296, Operations/Cycle: 3.117877


## Large

    python train_ahmad_q.py    --dct_coefficient_count 40 --window_size_ms 30 --window_stride_ms 10  --model_architecture ds_cnn_q --model_size_info 6 276 10 4 2 1 276 3 3 2 2 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 --quantize True --summary_dir work_q/DS_CNN_403010/retrain_log --train_dir work_q/DS_CNN_403010/training/ --learning_rate 0.0005 --how_many_training_steps 3000 
    python freeze___ahmad_q.py --dct_coefficient_count 40 --window_size_ms 30 --window_stride_ms 10  --model_architecture ds_cnn_q --model_size_info 6 276 10 4 2 1 276 3 3 2 2 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 276 3 3 1 1 --quantize True --start_checkpoint /tmp/speech_commands_train/best/ds_cnn_q_8980.ckpt-800 --output_file work_q/ds_cnn_q_frozen_quant.pb
    tflite_convert --graph_def_file work_q/ds_cnn_q_frozen_quant.pb --output_file work_q/ds_cnn_q_frozen_quant.tflite --input_format=TENSORFLOW_GRAPHDEF --output_format=TFLITE --input_arrays=Reshape_1 --output_arrays=labels_softmax --inference_type=QUANTIZED_UINT8 --mean_values 128 --std_dev_values 0.51619

    TFLITE
    Test:  Accuracy: 92.77%                                 Validation:  Accuracy: 92.75%
    [[408   0   0   0   0   0   0   0   0   0   0   0]      [[371   0   0   0   0   0   0   0   0   0   0   0]
     [  1 337   0  12   3   5  15  16   5   2   7   5]       [  0 300   3   9   1   4  13  23   3   1  13   1]
     [  1   3 405   0   1   0   7   0   0   0   2   0]       [  2   4 384   1   0   0   6   0   0   0   0   0]
     [  0   5   0 390   0   5   1   0   0   0   3   1]       [  2   5   3 379   0   5   5   1   0   0   1   5]
     [  1   5   0   0 389   0   3   1   1  11  14   0]       [  1   5   1   1 315   1   4   1   0   8  12   1]
     [  1   9   1  24   0 358   5   1   0   0   5   2]       [  1   1   2  10   0 350   2   0   0   0  10   1]
     [  1   2   2   1   0   0 405   1   0   0   0   0]       [  0   3   2   2   0   0 340   5   0   0   0   0]
     [  1   5   0   0   0   1   6 381   0   0   2   0]       [  0   5   3   0   0   0   1 353   0   0   0   1]
     [  2  12   0   0   3   4   1   0 346  23   3   2]       [  2   7   2   0   4   0   3   0 329  15   0   1]
     [  1   6   0   3  16   1   3   0   1 364   3   4]       [  1   1   1   0  20   0   3   2   2 334   7   2]
     [  0   2   0   0   0   0   3   0   0   0 406   0]       [  3   0   0   0   6   0   0   2   0   2 337   0]
     [  2  16   1  18   1   6   2   1   0   1   7 347]]       [  0   7   1  14   2  10   1   0   0   2   5 330]]

### GAP

Generate samples:

	python utils/generate_samples_images.py --dct_coefficient_count 40 --window_size_ms 30 --window_stride_ms 10

Run inference:

	make clean all run platform=gvsoc SMALL=1

Performances:

                    S1_Conv2d_276x1x10x4_Relu: Cycles:   17633372, Operations:   22179360, Operations/Cycle: 1.257806
                     S2_Conv2d_276x1x3x3_Relu: Cycles:     988987, Operations:    1380000, Operations/Cycle: 1.395367
                   S3_Conv2d_276x276x1x1_Relu: Cycles:    3520441, Operations:   38088000, Operations/Cycle: 10.819099
                     S4_Conv2d_276x1x3x3_Relu: Cycles:     357840, Operations:    1380000, Operations/Cycle: 3.856472
                   S5_Conv2d_276x276x1x1_Relu: Cycles:    3522691, Operations:   38088000, Operations/Cycle: 10.812189
                     S6_Conv2d_276x1x3x3_Relu: Cycles:     357554, Operations:    1380000, Operations/Cycle: 3.859557
                   S7_Conv2d_276x276x1x1_Relu: Cycles:    3520868, Operations:   38088000, Operations/Cycle: 10.817787
                     S8_Conv2d_276x1x3x3_Relu: Cycles:     381909, Operations:    1380000, Operations/Cycle: 3.613426
                   S9_Conv2d_276x276x1x1_Relu: Cycles:    3675423, Operations:   38088000, Operations/Cycle: 10.362889
                    S10_Conv2d_276x1x3x3_Relu: Cycles:     357884, Operations:    1380000, Operations/Cycle: 3.855998
                  S11_Conv2d_276x276x1x1_Relu: Cycles:    3522190, Operations:   38088000, Operations/Cycle: 10.813726
                        S12_AveragePool_25x20: Cycles:      46264, Operations:     138000, Operations/Cycle: 2.982881
                        S13_Linear_12x276x1x1: Cycles:       2663, Operations:       3312, Operations/Cycle: 1.243710
                                  S14_SoftMax: Cycles:       1613, Operations:         12, Operations/Cycle: 0.007440

                                        Total: Cycles:   37889699, Operations:  219660684, Operations/Cycle: 5.797372


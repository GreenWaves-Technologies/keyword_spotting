# Keyword Spotting

In this project you will find the deployment of a Keyword spotting application on Greenwaves-Technologies GAP processors. The algorithms are inspired by the [Google Speech commands example](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/examples/speech_commands) and [ARM KWS project](https://github.com/ARM-software/ML-KWS-for-MCU) adapted for GAP chips.

In the ML-KWS-for-GAP-training you will find the project for the neural networks training. To download and reproduce the results:

    git submodule update --init --recursive

The pretrained models are in the *model* folder already quantized and converted to .tflite format.


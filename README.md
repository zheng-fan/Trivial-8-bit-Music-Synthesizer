This is an 8-bit music synthesizer including C++ and JavaScript version without any other library.

# Features

* Wav audio format encoder
* Using pulse, triangle and noise waveform to get 8-bit style music
* The JavaScript version can be used on web
* Using FamiTracker's text sheet music format

# Usage

For C++ version, firstly, compose a music in FamiTracker and export it to a text file. Then, just compile and run `synth.cpp`. If you don't have a FamiTracker, you can use the scores in the `score` directory to have a test.

For JavaScript version, you have to use the tool in `tools` directory to transform FamiTracker text format score to a simple DSL first. You can get more information in my blog below.

# Details and Demo

You can get more details by browsing [my blog](https://fanzheng.org/archives/37) (in Chinese) and there is a demo of web version at the bottom of the post.

import pyaudio
import math
import numpy as np
import scipy.fftpack

CHUNK = 4096 # number of data points to read at a time
RATE = 44100 # time resolution of the recording device (Hz)
lowerFreqLimit = 50 #Lowest frequency of interest

numberBins = np.uint16(math.log(CHUNK,2))
frequencies = np.uint16((2**(np.arange(numberBins)))*RATE/CHUNK)

minFreq = 0

for i in range(numberBins):
    if(frequencies[i] < lowerFreqLimit):
        minFreq = i +1

p=pyaudio.PyAudio() # start the PyAudio class
stream=p.open(format=pyaudio.paInt16,channels=1,rate=RATE,input=True,
              frames_per_buffer=CHUNK) #uses default input device

def freqAnalizer():

    data = np.fromstring(stream.read(CHUNK),dtype=np.int16) #Stores CHUNK pressure values in data

    sample = scipy.fftpack.fft(data)    #Runs fourrier transform and returns the result in the complex plane
    freqOutput =  2.0/CHUNK * np.abs(sample[:CHUNK//2]) #gets ride of complex values in off bins
    freqBin = np.zeros(11)      #makes 11 bins to store Hz ranges 
    freqBin[0] = freqOutput[0]

    for w in range(1,11):
        a = np.uint16(2**(w-1))
        b = np.uint16(2**w)
        freqBin[w] = (freqOutput[a:b].sum())/(b-a)

    return freqBin[minFreq:]

output = freqAnalizer()

# close the stream gracefully
stream.stop_stream()
stream.close()
p.terminate()

### Opuspy

A simple wrapper over Opus allowing to write and read Opus files in Python.

#### Installation:

```pip3 install opuspy```

Wheels available for linux.

#### Docs:

```python
def read(path: str) -> Tuple[numpy.ndarray[numpy.int16], int]:
    """Returns the waveform_tc as the int16 np.array of shape [samples, channels] 
    and the original sample rate. NOTE: the waveform returned is ALWAYS at 48khz
    as this is how opus stores any waveform, the sample rate returned is just the 
    original sample rate of encoded audio that you might witch to resample the returned 
    waveform to."""

def write(
        path: str, 
        waveform_tc: numpy.ndarray[numpy.int16], 
        sample_rate: int, 
        bitrate: int = -1000, 
        signal_type: int = 0, 
        encoder_complexity: int = 10) -> None:
    """Saves the waveform_tc as the opus-encoded file at the specified path. 
    The waveform must be a numpy array of np.int16 type, and shape [samples (time axis), channels]. 
    Recommended sample rate is 48000. You can specify the bitrate in bits/s, as well as
    encoder_complexity (in range [0, 10] inclusive, the higher the better quality at given bitrate, 
    but more CPU usage, 10 is recommended). Finally, there is signal_type option, that can help to
    improve quality for specific audio, types (0 = AUTO (default), 1 = MUSIC, 2 = SPEECH)."""
```
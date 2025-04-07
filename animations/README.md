### Installation

1. Install required dependencies (Linux).
```
sudo apt update
sudo apt install -y libcairo2-dev libpango1.0-dev pkg-config python3-dev
```

2. Install project python packages.
```
pip install -r requirements.txt
```

### Testing

1. Run manim render (quick rendering example)
```
manim -ql tree_visual.py HorizontalTreeVisualizer
```

2. Go to /animations/media/videos/tree_visual/480p15..

3. Take a look at the output video animation!

## Export

1. Run manim render (1080p60 rendering example)
```
manim -qh tree_visual.py HorizontalTreeVisualizer
```

2. Go to /animations/media/videos/tree_visual/1080p60..

3. Take a look at the output video animation!

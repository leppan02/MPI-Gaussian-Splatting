# Download repo
```
$ git clone --recurse-submodules git@github.com:leppan02/MPI-Gaussian-Splatting.git
```

# Download image
https://drive.google.com/file/d/14kY69Fo5Lsz0Z-Ap8HxWHxAHhwtNKyRl/view?usp=sharing

and place in repo in folder

```
data/point_cloud.ply
```

# Run locally with mpirun
```
$ python3 run.py
$ python3 render_images.py
```

# Run on dardel
```
$ ./compile_dardel.sh
$ ./submit_dardel.sh
```


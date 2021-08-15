Convolution forms a pillar in a large number of image processing applications. It is a mathematical 
operation which is applied in a same manner over the entire considered region in an image. 
Accessing all pixels in a similar manner creates the room for optimization in this algorithm. 
We have thus targeted our efforts towards improving the spatial access of pixel elements during 
2D convolution and correlation.

There are many ways of performing 2D convolution, the naive one includes irregular access of source 
image pixels. Due to this, a large number of cache misses occur which further in turn degrade the 
performance. We have tried to improve this access pattern and hence reduce the number of overall 
cache misses.

What are we doing differently?

Our strategy involves storing relevant 2D image data in a 1D buffer container. This container will 
then be used for accessing required patches of pixels during convolution. These patches will be 
accessed by a sliding window algorithm so that their dot product with 1D aligned kernel can be stored 
in destination image.

But how much data should be stored inside the buffer?

We store only that much data which will be required by kernel during its one single horizontal slide. 
In other words, the size of buffer container will be kernel_size * img_width + boundary_offset. 
Here boundary_offset is the addition in buffer size which will take place due to boundary 
extrapolation for corner pixels.

What to do after forming the buffer container?

Once our data is present inside the buffer container, a sliding window algorithm is applied for 
calculating the dot product between kernel and image patch contained inside sliding window.

Boundary extrapolation is taken care by changing elements of buffer container suitably.

How do we take care of variable anchor point positions?

While filling the buffer container, we will take care of this by starting/ending the filling process 
at suitable points in source image.

This will automatically take care of the problem and will achieve desired effect due to the followed 
pattern of pixel assignment.

Two dimensional convolution and correlation
-------------------------------------------


What is the role of convolution in image processing?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Convolution forms a base pillar in a large number of image processing applications. It is a 
mathematical operation which is applied in a same manner over the entire considered region in an image. 
Accessing all pixels in a similar manner creates the room for optimization in this algorithm. 
We have thus targeted our efforts towards improving the spatial access of pixel elements during 
2D convolution and correlation.


---------------------------------


What are we doing differently?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


There are many ways of performing 2D convolution, the naive ones includes irregular access of source 
image pixels. Due to this, a large number of cache misses occur which further in turn degrade the 
performance. We have tried to improve this access pattern and hence reduce the number of overall 
cache misses.


Our strategy involves storing relevant 2D image data in a 1D buffer container. This container will 
then be used for accessing required patches of pixels during convolution. These patches will be 
accessed by a sliding window algorithm so that their dot product with 2D kernel can be stored 
in destination image.


---------------------------------


How much data should be stored inside the buffer?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


We store only that much data which will be required by kernel during its one single horizontal slide. 
In other words, the size of buffer container will be 

``buffer size = kernel_size * img_width + boundary_offset``

where ``boundary_offset = (extrapolation_required == 1) ? ((kernel_size - 1) * kernel_size) : 0``

Here, boundary_offset is the addition in buffer size which will take place due to boundary 
extrapolation for corner pixels.


----------------------------------


What to do after forming the buffer container?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Once our data is present inside the buffer container, a sliding window algorithm is applied for 
calculating the dot product between kernel and image patch contained inside sliding window.
Boundary extrapolation is taken care by changing elements of buffer container suitably.


----------------------------------


How do we take care of variable anchor point positions?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


While filling the buffer container, we will take care of this by starting/ending the filling process 
at suitable points in source image.
This will automatically take care of the problem and will achieve desired effect due to the followed 
pattern of pixel assignment.


-------------------------------------------------------


What to do with spatial separable kernels?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


We have written a custom algorithm which can detect whether a two dimensional kernel can be 
spatially separated. It does separation as well as checking for separability 
simultaneously. If at any step during checking, the kernel is deemed non-separable, the separation 
algorithm stops, otherwise horizontal and vertical separated kernels are returned.
The algorithm uses the fact that a separable 2D matrix is essentially a rank1 matrix. It further 
uses basic linear algebra for separating the kernel.


-------------------------------------------------------


A note regarding derivatives : 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Calculating derivatives with different boundary options may produce artifacts in some cases. Hence 
it is advisable to compare outputs obtained with all methods of boundary extrapolation and then 
select the one which seems most reasonable for the application.

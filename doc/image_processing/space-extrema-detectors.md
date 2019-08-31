## Space extrema detectors

### What is being detected?

A good feature is one that is repeatable, stable and can be recognized under affine transformations. Unfortunately, edges do not fit the description. Corners, on the hand, fit well enough.

---

### Available detectors

At the moment, the following detectors are implemented

 - Harris corner detector

 - Hessian detector

---

### Algorithm steps

#### Harris and Hessian

Both are derived from a concept called Moravec window. Lets have a look at the image below:

\<Moravec-window-image>

As can be noticed, moving the yellow window in any direction will cause very big change in intensity. This is the key concept in understanding how the two corner detectors work.

The algorithms have the same structure:

 1. Compute image derivatives

 2. Weighted convolution

 3. Compute response

 4. Threshold (optional)

Harris and Hessian differ in what **derivatives they compute**. Harris computes the following derivatives:

$HarrisMatrix = [(dx)^2, dxdy], [dxdy, (dy)^2]$

*(note that $d(x^2)$ and $(dy^2)$ are **numerical** powers, not gradient again).*

The three distinct terms of a matrix can be separated into three images, to simplify implementation. Hessian, on the other hand, computes second order derivatives:

$HessianMatrix = [dxdx, dxdy][dxdy, dydy]$

**Weighted convolution** is the same for both. Usually Gaussian blur matrix is used as weights, because corners should have hill like curvature in gradients, and other weights might be noisy.

**Response computation** is a matter of choice. Given the general form of both matrices above

$[a, b][c, d]$

One of the response functions is

$response = det - k * trace^2 = a * c - b * d - k * (a + d)^2$

$k$ is called discrimination constant. Usual values are $0.04$ - $0.06$.

The other is simply determinant

$response = det = a * c - b * d$

**Thresholding** is optional, but without it the result will be extremely noisy. For complex images, like the ones of outdoors, for Harris it will be in order of \<Harris-value> and for Hessian will be in order of \<Hessian value>. For simpler images values in order of 100s and 1000s should be enough.

To get deeper explanation please refer to following **papers**:

\<Harris paper>

\<Hessian matrix>

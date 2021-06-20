# Task 4
### Thresholding
##### Global Atsu
```c++
std::vector<int> globalAtsu(Image &inputImg, int histSize = 256, int numModes = 2);
```
function parameters:
* inputImg: reference to gray Image
* histSize: histogram size depending on the number of bits that represent each pixel in the image
* numModes: number of modes

function return: return array of thresholds depending on number of modes

**Results**:<br>
* global atsu 2 modes
  ![](Resources/images/atsu-global-2-modes.png)

* global atsu 3 modes
  ![](Resources/images/atsu-global-3-modes.png)

##### Local Atsu
```c++
Image localAtsu(Image &inputImg, int blockDim, int histSize = 256, int numModes = 2);
```
function parameters:
* inputImg: reference to gray Image
* blockDim: block dimension (eg, 5 means 5 x 5)
* histSize: histogram size depending on the number of bits that represent each pixel in the image
* numModes: number of modes

function return: return the thresholded image

**Results**:<br>
* local atsu 2 modes, blockDim: 5
  ![](Resources/images/atsu-local-2-modes.png)

* local atsu 3 modes, blockDim: 5
  ![](Resources/images/atsu-local-3-modes.png)
##### Global Optimal Iterative Thresholding

```c++
int globalOptimalIterativeThresholding(Image &inputImg);
```
function parameters:
* inputImg: reference to gray Image

function return: return threshold

**Results**:<br>
![](Resources/images/global-optimal.png)

##### Local Optimal Iterative Thresholding
```c++
Image localOptimalIterativeThresholding(Image &inputImg, int blockDim);
```
function parameters:
* inputImg: reference to gray Image
* blockDim: block dimension (eg, 5 means 5 x 5)

function return: return threshold

**Results**:<br>
blockDim: 7
![](Resources/images/local-optimal.png)

##### Means-Shift
```c++
MeanShift(Image *image, float hs, float hr)
```
class functions:
* ```c++ float distance(int i1, int j1, int i2, int j2```
* ```c++ float distance(vector<float> pt1, vector<float> pt2)```
* ```c++ Image run()```


**Results**:<br>
blockDim: 7
![](Resources/images/Mean-Shift.jpg)

##### K-Mean
```c++
K_mean(Image *image, int k, int maxIteration)
```
class functions:
* ```c++ void clusterPixels()```
* ```c++ int closestCluster(int i, int j)```
* ```c++ void getClustersCentroid()```
* ```c++ bool centroidsChanged(vector<vector<float>> oldCentroids)```
* ```c++ float distance(int i, int j, vector<float> centroid)```
* ```c++ void run()```
* ```c++ K_mean()```
* ```c++ Image getOutput()```

##### Region Growing
```c++
Image RGSegmentation(Image & inputImg, std::vector<point> seedPoint)

```
--> You should select seed points from image in gui to segment the regions of these seed points

function parameters:
* inputImg: reference to color image
* seedPoint: Points which are used as seeds for segmentation regions

function return: return segmented output image
**Results**:<br>
blockDim: 7
![](Resources/images/Region-GRowing.jpg)

##### Agglomerative
```c++
Image agglomerativeSeg(Image & img, int numOfClusters, unsigned long initialClustersNum)
```
function parameters:
* img: reference to gray image
* numOfClusters : the number of clusters needed in the output image
* initialClustersNum : The initial numbers of clusters which is merged until reach to the the numOfClusters

function return: return threshold
**Results**:<br>
blockDim: 7
![](Resources/images/Agglomerative.jpg)


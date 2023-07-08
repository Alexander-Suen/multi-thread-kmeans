1. makefile :
文件使用了matplotlibcpp以及thread等库，所以需要调用相应的包，编译的时候需要加上一些选项。
`g++ KM_simulation.cpp -I/home/artanis-tera/Github/matplotlib-cpp -I/usr/include/python3.8 -lpython3.8 -o kmeans`

2. matplotlibcpp.h
里面有我们需要画图的头文件。

3. KM_simulation.cpp是主要的程序。

这里面并行分为两个部分，首先，kmeans算法的工作流程如下：	
（1）首先给出k个点代表k类，n的点代表要分类的数据，重复以下过程
（2）n个点中，对于任何点a，距离k个点中的那个点b近，这个a点就属于b
（3）根据分类结果重新计算k个点，即，每一类的中心点就是新的类别代表点。

那么，通过Kmeans执行过程的分析，并行计算可以处理（2）和（3）。

方法如下：
	问题： n个点分为k类，现在给出k个点的位置。
	对于（2）：
	建立k个线程，每个线程计算n个点到k的点中某一个点的距离，并在计算的过程中计算出最小距离。
	对于（3）：
	我们先对分好类的点集中起来，方便线程对数据的调用。
	我们建立k个线程处理被分类的点，每一类使用一个线程计算此类点的中心为多少，并更新k。

4. Image, 以图片形式存放最终实验结果。NO_SCREEN为true时生效。





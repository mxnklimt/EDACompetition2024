聚类算法
修改聚类算法的特征值数量
同时将生成的数据保存到文件中
新建一个容器，每一个簇和其中的点的数据放进去
计算每个簇的中心位置并放置buff
随后编写算法在中心的周围寻找点
找到放置点
可以看出，有些点找到了near center
但是打印出error就说明并没有找到附近的点
修改搜索半径为1000，测试了一下百分百能找到
然而同门突然告诉我，ff也有size。。。。他之前告诉我这个值不用管
也就是点也是有x和y的，也就是说也有长宽。。而且还不小
结果就是。。重写重叠判断
同时发现一个问题
搜索半径调到2000也经常找不到合适的buff放置点了
首先优化寻找流程
如果搜索半径不够用再增加，减少运算量
然而debug以后发现这么写逻辑是错的
max_radius确实变大了，但是当前点他也不判断了
直接输出error跑到下一个点去了
于是只能重写逻辑
成功了，但是又有个新的问题
只判断了单独的一个簇里的点和缓冲区（buff）是否重叠。具体来说，`isOverlap` 函数和 `findNonOverlappingPosition` 函数都是针对单个簇（cluster）进行判断的。
修复了上述问题
修改了重叠判断逻辑并且使用了动态搜索范围
但是七分钟跑了56个数据。。。并且把我内存吃满了
指数增大搜索半径，搜索速度提高百分之25
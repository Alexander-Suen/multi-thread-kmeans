// 头文件来源。
// https://github.com/lava/matplotlib-cpp
//



/*
 *  注意！使用前先清理Image文件夹
 *  输入数据 
 *  分类数 点数 最大迭代次数
 *  当点数为0的时候表示手动输入点,否则表示随机生成n个点。
 *  NO_SCREEN 用于修改是否在屏幕上实时显示结果。如果不用则会将结果以图片的形式保存在pic中
 *  NO_SCREEN默认为true，因为实时显示消耗一些时间，影响性能。
 *
 *  编译的时候首先修改以下Makefile中的INCLUDE中的/usr/include/python3.8
 *
 * */
#include <atomic> // 原子操作
#include <iostream>
#include <math.h>
#include <matplotlibcpp.h> // 可视化
#include <set>
#include <stdlib.h>
#include <string>
#include <thread> // 线程调用
#include <time.h>
#include <vector>

#define NO_SCREEN true
// NO_SCREEN为true表示将结果显示为图片并且存到Image文件夹中
// NO_SCREEN为false表示将结果实时显示。由于实时显示耗费资源较大，通常采用图片显示

using namespace std;
namespace plt = matplotlibcpp; // C++ plotting library

typedef vector<double> double_list;
template <typename T> struct atomwrapper {  // vector无法和atomic连用，所以这里使用一个结构体包裹atomic。
  std::atomic<T> _a; // 定义原子操作，执行中不可被中断
  atomwrapper() : _a() {}
  atomwrapper(const std::atomic<T> &a) : _a(a.load()) {}
  atomwrapper(const atomwrapper &other) : _a(other._a.load()) {}
  atomwrapper &operator=(const atomwrapper &other) {
    _a.store(other._a.load());
  }
};

class kmeans {
private:
  int n, k, max_iter;
  atomic<bool> iter_flag;
  double_list x, y, z, kx, ky;
  vector<atomwrapper<int>> n_kind;  // n_kind表示n个点中每个点属于哪一类
                                    // 多个线程可能会访问临界资源这个时候需要1.
                                    // 互斥操作， 2. atomic也可以。
  vector<vector<int>> set_k;        // k类中，每一类有哪些点。
  vector<string> k_color, n_color;  // k类颜色，n个点每个点的颜色
  static double thr;

public:
  void display(int iter_generation) { // 展示实验结果
    renew_color();            // 首先给每个点上色
    plt::cla();               // 清空画板，否则k点一直存在
    plt::scatter_colored(x, y, n_color);  // 画出n个点
    plt::scatter_colored(kx, ky, k_color, 60);    // 画出k个点
    if (NO_SCREEN) { // 以图片保存结果
      char filename[100];
      sprintf(filename, "Image/kmeans%d.png",iter_generation);
      plt::save(filename); // 保存实验结果文件
    } else
      plt::pause(0.01);     // 展示0.01秒
  }
  string getcolor(double col) {   // 获取颜色. RGB - 256 * 256 * 256
    // 将5000 -- 255 * 255 * 255 - 5000 映射到 0 -- 1之间的小数
    // 其中任何一个数字转换为十六进制，对应某一种颜色
    int col_ = (double)col * (pow(255, 3) - 100000) + 5000;
    string ans = "#";
    for (int i = 0; i < 6; i++) { // 转换为十六进制
      if (col_ % 16 < 10) {
        ans += (col_ % 16) + '0';
      } else {
        ans += ((col_ % 16) - 10) + 'a';
      }
      col_ /= 16;
    }
    return ans;
  }
  kmeans(int k, int max_iter, int n = 0) : k(k), max_iter(max_iter), n(n) {

    //构造函数，用于随机生成n个点或者手动获得一些点。
    double temp_xy[2];
    if (n == 0) {
      printf("input point,Ctrl-D finish input\n");
      while (scanf("%lf%lf", &temp_xy[0], &temp_xy[1]) == 2) {
        x.push_back(temp_xy[0]);
        y.push_back(temp_xy[1]);
        std::atomic<int> a_i(0);
        n_kind.push_back(a_i);
        this->n++;
      }
    } else {
      for (int i = 0; i < n; i++) {
        std::atomic<int> a_i(0);
        n_kind.push_back(a_i);
        x.push_back(((double)1 / (double)n) * (rand() % n));
        y.push_back(((double)1 / (double)n) * (rand() % n));
      }
    }

    // 生成初始的k个点
    set<int> temp_posi;
    int i = 0;
    srand(time(NULL));
    while (i < k) {
      vector<int> temp_set;
      int posi = rand() % this->n;
      if (temp_posi.count(posi) == 1)
        continue;
      kx.push_back(x[posi]);
      ky.push_back(y[posi]);
      set_k.push_back(temp_set);
      temp_posi.insert(posi);
      k_color.push_back(getcolor(i)); // 将生成的k个点着色
      i++;
    }
    //display(1);
  }

  double len(int ni, int ki) {

    //用于计算n个点中的第ni个点到k中的ki个点的距离。
    return sqrt(pow(x[ni] - kx[ki], 2) + pow(y[ni] - ky[ki], 2));
  }
  void n_to_kind(int id) { // 分类，将n个点分为k类
    for (int i = 0; i < n; i++) {
      if (len(i, n_kind[i]._a) > len(i, id)) {
        n_kind[i]._a = id;
      }
    }
  }
  void renew_k(int id) {    // 根据n个点分类好的结果重新计算k个点的位置
    if (set_k[id].size() == 0)
      return;
    double pre_x = kx[id], pre_y = ky[id];
    kx[id] = 0;
    ky[id] = 0;
    for (vector<int>::iterator iter = set_k[id].begin();
         iter != set_k[id].end(); iter++) {
      kx[id] += x[*iter];
      ky[id] += y[*iter];
    }
    kx[id] /= set_k[id].size();
    ky[id] /= set_k[id].size();
    if (sqrt(pow(kx[id] - pre_x, 2) + pow(ky[id] - pre_y, 2)) > this->thr)
      iter_flag = false;
  }
  void renew_color() {
    // 更新n个点的颜色，用于后面的作图。
    n_color.clear();
    for (int i = 0; i < n; i++) {
      n_color.push_back(k_color[n_kind[i]._a]); // n_kind[i]：n个点中的第i个点所属的类别
    }
  }
  void kmeans_iteration() // kmeans迭代
  {
    /*
     * 1. 计算n个点中每个点到k个点的距离，对n个点进行分类
     * 2. 得到分类之后重新计算k个点。
     */
    int iter_num = 0;
    iter_flag = false;
    while (iter_num < max_iter) {
      if (iter_flag)
        break;
      iter_flag = true;
      vector<std::thread> t;
      for (int i = 0; i < k; i++) { // n点分为k类
        set_k[i].clear();
        t.push_back(std::thread(&kmeans::n_to_kind, this, i));  // 开线程处理函数
      }
      for (int i = 0; i < k; i++)
        t[i].join();    // 同步点

      for (int i = 0; i < n; i++) {
        set_k[n_kind[i]._a].push_back(i);
      }
      t.clear();


      for (int i = 0; i < k; i++) { // 重新计算k
        t.push_back(std::thread(&kmeans::renew_k, this, i));    // 开线程处理函数。
      }
      for (int i = 0; i < k; i++)
        t[i].join();  //同步点
      display(iter_num);
      iter_num++;
    }
    printf("\niter number is %d\n"
           "finish iter\n", iter_num);
    plt::close();
    exit(0); // zomb
  }
};

double kmeans::thr = 1e-3;  // 阈值，每个点变化量小于这个值就会停止迭代
// 每个点的坐标[0,1]之间。
int main() {
  int n, k, max_iter;
  printf("input k,n,max_iter\n(if n = 0 ,we input n point by hand ,otherwise by random) :\n");
  scanf("%d%d%d", &k, &n, &max_iter);
  kmeans a(k, max_iter, n); // n == 0 从输入中得到点的坐标。
  a.kmeans_iteration();     // 计算结果
  pthread_exit(NULL);
}

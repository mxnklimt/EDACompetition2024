#pragma once
#include <vector>
#include <utility>
#include <future>
#include "ThreadPool.h" 
#include "ffdot.h" 
#include "isOverlap.h" 
#include "Manhattan_distance.h"
std::pair<int, int> findNonOverlappingPosition(int center_x, int center_y, int buff_width, int buff_height, 
    const std::vector<std::vector<Point>>& clusters, int x_max, int x_min, int y_max, int y_min, 
    std::vector<std::vector<Point>>& clusters_buff1) {
    //buff_flag==1,��ֻ��Ҫ�ж�clusters_buff1��clusters�Ƿ�������buff�ص�
    //buff_flag==2,����Ҫ�ж�clusters_buff2��clusters_buff1��clusters�Ƿ�������buff�ص�
    const int initial_step = 100;//��ʼ������
    int max_radius = 300;//��ʼ��󻯰뾶
    const double radius_multiplier = 2;//�뾶��������
    ThreadPool pool(7);//����һ��7�߳��̳߳�
    std::vector<std::future<std::pair<int, int>>> results;//����һ������std::future<std::pair<int, int>> �����������ڴ洢�첽����Ľ��

    while (true) {
        for (int radius = 100; radius <= max_radius; radius = radius * 2) {  //��radius�뾶100��ʼ��ÿ��*2
            int step = initial_step + radius / 10;//�����ʼ����
            for (int dx = -radius; dx <= radius; dx += step) {//����forѭ�������߳�Ϊradius�������Σ�������������step
                for (int dy = -radius; dy <= radius; dy += step) {
                    results.emplace_back(pool.enqueue([=, &clusters] {//��һ���첽������ӵ��̳߳��У�pool.enqueue:
                    //[=, &clusters]:����һ�������б�����ָ�� lambda ������β����ⲿ������[=] ��ʾ��ֵ���������ⲿ������&clusters ��ʾ�����ò��� clusters ������������Ϊ clusters ������һ���ϴ�����ݽṹ�������ò�����Ա��ⲻ��Ҫ�Ŀ�����
                    //�� lambda �������������ⲿ���������� center_x, center_y, buff_width, buff_height, initial_step, max_radius, radius_multiplier, dx, dy �ȣ���ֵ���񣬲������ò��� clusters��
                    //��� lambda ��������һ���������߳���ִ�С�    
                        int new_x = center_x + dx;//�½�buff������
                        int new_y = center_y + dy;

                        if ((!isOverlap(new_x, new_y, buff_width, buff_height, clusters)) &&
                            (!isOverlap(new_x, new_y, buff_width, buff_height, clusters_buff1)) &&
                            (new_x > x_min) && (new_x < x_max) && (new_y > y_min) && (new_y < y_max)) {
                            //����½�buff�Ƿ������еĵ���ײ�ص�,���buff�Ƿ�������buff�ص������buff�Ƿ񰲷�������area��
                            
                            return std::make_pair(new_x, new_y);//������ص��������½�buff������
                            
                            
                        }
                        return std::make_pair(-1, -1);//����ص������أ�-1��-1��
                        }));
                }
            }
        }

        for (auto&& result : results) {
            //ʹ�� for (auto&& result : results) �������� results��
            //auto ���Զ������Ƶ��Ĺؼ��֣�����������ݱ��ʽ�������Զ��Ƶ��� result ��ʵ�����͡�
            //result : results��result �� results �����е�ÿ��Ԫ�ء�
            auto pos = result.get();// ��ȡ�첽����Ľ��
            //auto����result.get()�Զ��Ƶ���pos������
            //pos �������� std::pair<int, int>����ʾ�ҵ��ķ��ص�λ�õ�����
            //�������������ǰ�̣߳�ֱ���첽������ɣ�����������Ľ����
            if (pos.first != -1 && pos.second != -1) {// ������Ƿ���Ч
                return pos;//����ҵ����ص�λ�ã����ظ�λ��
            }
        }
        results.clear();
        max_radius = static_cast<int>(max_radius * radius_multiplier);//����������е����˵��û���ҵ����ص�λ�ã������뾶����
    }
}
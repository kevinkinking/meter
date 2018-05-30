//
// Created by xukaiwe on 18-3-12.
//
#include "utools.h"

bool cmp(const vector<int>::iterator &a,const vector<int>:: iterator&b)
{
    return (*a) < (*b);
}

void rects_sort(const vector<Rect> rects, vector<int> &sorted_indexs, const int sort_index)
{
    vector<int> slice_vector;
    for(int i = 0;i < rects.size();i++){
        if(sort_index == 0){
            slice_vector.push_back(rects[i].x);
        }else if(sort_index == 1){
            slice_vector.push_back(rects[i].y);
        }else if(sort_index == 2){
            slice_vector.push_back(rects[i].x + rects[i].width);
        }else if(sort_index == 3){
            slice_vector.push_back(rects[i].y + rects[i].height);
        }

    }

    vector <vector<int>::iterator >vi;
    for(vector<int>::iterator it= slice_vector.begin();it!= slice_vector.end();it++)
        vi.push_back(it);

    sort(vi.begin(),vi.end(),cmp);
    for(vector<vector<int>::iterator  >::iterator it= vi.begin();it!= vi.end();it++)
        sorted_indexs.push_back(*it -slice_vector.begin());

}
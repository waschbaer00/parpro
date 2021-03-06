#ifndef _X_GRADIENT_PAR
#define _X_GRADIENT_PAR

#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>
#include <omp.h>
#include "x_gradient.h"

#include <thread>
#include <future>
#include <vector>

using namespace std;
using namespace boost::gil;


template <typename Out> struct halfdiff_cast_channels; // forward declaration


static int range_y = 8;// amount of computations is same for any task, because all pixels need one computation

//std::promise is meant to be used only once, so I would suggest to either create this set of promises every time.


//template <typename SrcView, typename DstView>
//void kernel(const SrcView& src, const DstView& dst, const int task)
//{
//	typedef typename channel_type<DstView>::type dst_channel_t;
//    int current_task = task;
//    int start_y = current_task * range_y;
//    int end_y = start_y + range_y;
//    if( end_y > src.height()) end_y = src.height();
//
//    for (int y = start_y; y < end_y; ++y)
//    {
//        typename SrcView::x_iterator src_it = src.row_begin(y);
//        typename DstView::x_iterator dst_it = dst.row_begin(y);
//
//        for (int x = 1; x < src.width() - 1; ++x)
//        {
//            static_transform(src_it[x - 1], src_it[x + 1], dst_it[x],
//                                 halfdiff_cast_channels<dst_channel_t>());
//        }
//    }
//}

template <typename SrcView, typename DstView>
void kernel(future<int&>&& f, const SrcView& src, const DstView& dst, int num_tasks)
{
    typedef typename channel_type<DstView>::type dst_channel_t;
    int current_task = f.get();
    while(current_task <= num_tasks) {

        int start_y = current_task * range_y;
        int end_y = start_y + range_y;
        if( end_y > src.height()) end_y = src.height;

        for (int y = start_y; y < end_y; ++y)
        {
            typename SrcView::x_iterator src_it = src.row_begin(y);
            typename DstView::x_iterator dst_it = dst.row_begin(y);

            for (int x = 1; x < src.width() - 1; ++x)
            {
                static_transform(src_it[x - 1], src_it[x + 1], dst_it[x],
                                 halfdiff_cast_channels<dst_channel_t>());
            }
        }

    }

}

//template <typename SrcView, typename DstView>
//void kernel(std::promise<int>&& p, std::future<int>&& f,const SrcView& src, const DstView& dst, int* init_task, int num_tasks)
//{
//    int current_task = *init_task;
//    while(current_task <= num_tasks) {
//        p.set_value(current_task + 1);
//
//        int start_y = current_task * range_y;
//        int end_y = start_y + range_y;
//        if( end_y > src.height()) end_y = src.height;
//
//        for (int y = start_y; y < end_y; ++y)
//        {
//            typename SrcView::x_iterator src_it = src.row_begin(y);
//            typename DstView::x_iterator dst_it = dst.row_begin(y);
//
//            for (int x = 1; x < src.width() - 1; ++x)
//            {
//                static_transform(src_it[x - 1], src_it[x + 1], dst_it[x],
//                                 halfdiff_cast_channels<dst_channel_t>());
//            }
//        }
//
//        current_task = f.get();
//    }
//
//}


template <typename SrcView, typename DstView>
void x_gradient(const SrcView& src, const DstView& dst, int num_threads) {

    static SrcView src_ = src;
    static DstView dst_ = dst;
    static int num_tasks = src.height()/range_y + 1;
    static int task_counter = -1;

    static vector<promise<int&>> p = vector<promise<int&>>(num_tasks);
    static vector<future<int&>> f;

    for(int i = 0 ; i < num_tasks; i++){
        f.push_back(p[i].get_future());
    }

    for(int i = 0; i < num_threads; i++){

        thread t(kernel<SrcView,DstView>,
                std::move(f[i]),
                 std::ref(src),std::ref(dst),num_tasks);

    }
//
//    for (int i = 0 ; i < num_threads ; ++i ) { threads[i].join(); }

}
#endif // !_X_GRADIENT_PAR_


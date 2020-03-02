#! /usr/bin/env bash

data_dir=/opt/data
homogr_dir=${data_dir}/cmp/homogr
bin_dir=../../build/bin

snn_threshold=0.6
ransac_max_iter=2000
ransac_inlier_thr=3.0
n_steps=1

sift_options="--sift-double-image --snn-threshold ${snn_threshold}"
ransac_options="--ransac-max-n-iterations ${ransac_max_iter} --ransac-inlier-threshold ${ransac_inlier_thr}"
benchmark_options="--benchmark-ransac --benchmark-steps ${n_steps}"
options="${sift_options} ${ransac_options} ${benchmark_options}"

>&2 printf "running ransac benchmark\n  sift_options=%s\n  ransac_options=%s\n  benchmark_options=%s\n" \
    "${sift_options}" "${ransac_options}" "${benchmark_options}"

printf "      dataset,     n,ni_avg,ni_std,ni_min,ni_lwq,ni_med,ni_upq,ni_max,  rt_avg,  rt_std,  rt_min,  rt_lwq,  rt_med,  rt_upq,  rt_max\n"
for iset in "adam.png boat.png BostonLib.png BruggeTower.png city.png Eiffel.png ExtremeZoom.png graf.png LePoint1.png LePoint2.png LePoint3.png" \
                "Boston.jpg BruggeSquare.jpg Brussels.jpg CapitalRegion.jpg WhiteBoard.jpg";
do
    for i in ${iset}
    do
        img_base=${i%.*}
        img_ext=${i##*.}
        img_left="${homogr_dir}/${img_base}A.${img_ext}"
        img_right="${homogr_dir}/${img_base}B.${img_ext}"
        printf "%13s," ${img_base}
        ${bin_dir}/nx-fit-homography -l ${img_left}  -r ${img_right} ${options}
    done
done

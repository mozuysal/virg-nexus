/**
 * @file nx_transform_2d.c
 *
 * Copyright (C) 2019,2020 Mustafa Ozuysal. All rights reserved.
 *
 * This file is part of the VIRG-Nexus Library
 *
 * VIRG-Nexus Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VIRG-Nexus Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_transform_2d.h"

#include <math.h>

/**
 * S = T(dx_post, dy_post) * lambda * R(theta) * T(dx_pre, dy_pre)
 *
 * @param t 3x3 transformation matrix
 * @param dx_post translation after warp
 * @param dy_post translation after warp
 * @param lambda scale
 * @param theta angle of in-plane rotation
 * @param dx_pre translation before warp
 * @param dy_pre translation before warp
 */
void nx_transform_2d_fill_similarity_s(float *t, float dx_post, float dy_post, float lambda,
                                       float theta, float dx_pre, float dy_pre)
{
        float lc = lambda*cos(theta);
        float ls = lambda*sin(theta);

        t[0] = lc;
        t[1] = ls;
        t[2] = 0.0f;
        t[3] = -ls;
        t[4] = lc;
        t[5] = 0.0f;
        t[6] = dx_post + dx_pre*lc - dy_pre*ls;
        t[7] = dx_pre*ls + dy_post + dy_pre*lc;
        t[8] = 1.0f;
}

/**
 * S = T(dx_post, dy_post) * lambda * R(theta) * T(dx_pre, dy_pre)
 *
 * @param t 3x3 transformation matrix
 * @param dx_post translation after warp
 * @param dy_post translation after warp
 * @param lambda scale
 * @param theta angle of in-plane rotation
 * @param dx_pre translation before warp
 * @param dy_pre translation before warp
 */
void nx_transform_2d_fill_similarity_d(double *t, double dx_post, double dy_post, double lambda,
                                       double theta, double dx_pre, double dy_pre)
{
        double lc = lambda*cos(theta);
        double ls = lambda*sin(theta);

        t[0] = lc;
        t[1] = ls;
        t[2] = 0.0;
        t[3] = -ls;
        t[4] = lc;
        t[5] = 0.0;
        t[6] = dx_post + dx_pre*lc - dy_pre*ls;
        t[7] = dx_pre*ls + dy_post + dy_pre*lc;
        t[8] = 1.0;
}

/**
im * A = T(dx_post, dy_post) * lambda * R(psi) * [ cos(theta)^-1 0; 0 1] * R(phi) * T(dx_pre, dy_pre)
 *
 * @param t 3x3 transformation matrix
 * @param dx_post translation after warp
 * @param dy_post translation after warp
 * @param lambda scale
 * @param psi angle of in-plane rotation
 * @param theta tilt angle
 * @param phi view angle (insignificant when theta is zero)
 * @param dx_pre translation before warp
 * @param dy_pre translation before warp
 */
void nx_transform_2d_fill_affine_s(float *t, float dx_post, float dy_post, float lambda,
                                   float psi, float theta, float phi,
                                   float dx_pre, float dy_pre)
{
        float sphi = sin(phi);
        float cphi = cos(phi);
        float spsi = sin(psi);
        float cpsi = cos(psi);
        float ctheta_inv = 1.0f/cos(theta);
        float lss = lambda*sphi*spsi;
        float lcc = lambda*cphi*cpsi;
        float lsc1 = lambda*spsi*cphi;
        float lsc2 = lambda*sphi*cpsi;
        float lssi = lss*ctheta_inv;
        float lcci = lcc*ctheta_inv;
        float lsc1i = lsc1*ctheta_inv;
        float lsc2i = lsc2*ctheta_inv;

        t[0] = -lss + lcci;
        t[1] = lsc2 + lsc1i;
        t[2] = 0.0f;
        t[3] = -lsc2i - lsc1;
        t[4] = -lssi + lcc;
        t[5] = 0.0f;
        t[6] = dx_post + dx_pre*(-lss + lcci) + dy_pre*(-lsc2i - lsc1);
        t[7] = dx_pre*(lsc2 + lsc1i) + dy_post + dy_pre*(-lssi + lcc);
        t[8] = 1.0f;
}

/**
 * A = T(dx_post, dy_post) * lambda * R(psi) * [ cos(theta)^-1 0; 0 1] * R(phi) * T(dx_pre, dy_pre)
 *
 * @param t 3x3 transformation matrix
 * @param dx_post translation after warp
 * @param dy_post translation after warp
 * @param lambda scale
 * @param psi angle of in-plane rotation
 * @param theta tilt angle
 * @param phi view angle (insignificant when theta is zero)
 * @param dx_pre translation before warp
 * @param dy_pre translation before warp
 */
void nx_transform_2d_fill_affine_d(double *t, double dx_post, double dy_post, double lambda,
                                   double psi, double theta, double phi,
                                   double dx_pre, double dy_pre)
{
        double sphi = sin(phi);
        double cphi = cos(phi);
        double spsi = sin(psi);
        double cpsi = cos(psi);
        double ctheta_inv = 1.0f/cos(theta);
        double lss = lambda*sphi*spsi;
        double lcc = lambda*cphi*cpsi;
        double lsc1 = lambda*spsi*cphi;
        double lsc2 = lambda*sphi*cpsi;
        double lssi = lss*ctheta_inv;
        double lcci = lcc*ctheta_inv;
        double lsc1i = lsc1*ctheta_inv;
        double lsc2i = lsc2*ctheta_inv;

        t[0] = -lss + lcci;
        t[1] = lsc2 + lsc1i;
        t[2] = 0.0;
        t[3] = -lsc2i - lsc1;
        t[4] = -lssi + lcc;
        t[5] = 0.0;
        t[6] = dx_post + dx_pre*(-lss + lcci) + dy_pre*(-lsc2i - lsc1);
        t[7] = dx_pre*(lsc2 + lsc1i) + dy_post + dy_pre*(-lssi + lcc);
        t[8] = 1.0;
}


/**
 * x[TL, TR, BR, BL] -> unit square[(0,0) - (1,0) - (1,1) - (0,1)]
 *
 * @param t 3x3 transformation matrix
 * @param corners Corner coordinates of a deformed unit-square, [x, y, x, y, x, y, x, y]
 *
 * @return determinant of the computation
 */
float nx_transform_2d_fill_perspective_s(float *t, const float *corners)
{
        float t1 = corners[0] * corners[3];
        float t2 = corners[0] * corners[5];
        float t3 = corners[4] * corners[3];
        float t4 = corners[2] * corners[1];
        float t5 = corners[4] * corners[1];
        float t6 = corners[2] * corners[5];
        float t8 = corners[2] * corners[7];
        float t9 = corners[6] * corners[5];
        float t10 = corners[6] * corners[3];
        float t11 = corners[7] * corners[4];
        float t12 = -t8 + t6 - t3 - t9 + t10 + t11;
        float t15 = corners[0] * corners[6];
        float t16 = corners[3]*corners[3];
        float t19 = 0.2e1f * t16 * t5 * t15;
        float t20 = t16 * corners[4];
        float t22 = corners[6] * t20 * t2;
        float t24 = corners[0] * corners[2];
        float t25 = corners[7]*corners[7];
        float t26 = corners[4] * t25;
        float t28 = corners[1] * t26 * t24;
        float t30 = corners[0]*corners[0];
        float t31 = corners[7] * t30;
        float t32 = corners[3] * t9;
        float t33 = t32 * t31;
        float t35 = corners[5] * t8;
        float t36 = corners[6] * corners[4];
        float t37 = corners[1] * t36;
        float t39 = 0.2e1f * t37 * t35;
        float t41 = corners[7] * t10;
        float t43 = 0.2e1f * t41 * corners[4] * t2;
        float t44 = corners[4]*corners[4];
        float t45 = t44 * corners[7];
        float t46 = corners[6] * t16;
        float t48 = corners[7] * corners[0];
        float t50 = corners[1] * t10;
        float t51 = t50 * corners[4] * t48;
        float t53 = corners[6] * corners[2];
        float t54 = corners[5] * t53;
        float t55 = corners[3] * t5;
        float t56 = t55 * t54;
        float t58 = corners[2]*corners[2];
        float t59 = corners[7] * t58;
        float t60 = corners[5]*corners[5];
        float t61 = corners[6] * t60;
        float t63 = t60 * t58;
        float t64 = corners[6] * corners[1];
        float t65 = t64 * t63;
        float t67 = t60 * corners[0];
        float t68 = corners[3] * t53;
        float t69 = t68 * t67;
        float t71 = corners[1]*corners[1];
        float t72 = t71 * corners[6];
        float t74 = corners[6]*corners[6];
        float t75 = t74 * corners[5];
        float t76 = corners[4] * t71;
        float t78 = -t19 + 0.2e1f * t22 + 0.2e1f * t28 + 0.2e1f * t33 + t39 - t43 + t46 * t45 + 0.2e1f * t51 - 0.2e1f * t56 - t61 * t59 + 0.2e1f * t65 - 0.2e1f * t69 + t72 * t45 - t76 * t75;
        float t79 = corners[2] * t71;
        float t80 = t79 * t75;
        float t82 = corners[5] * t24;
        float t83 = corners[7] * t3;
        float t84 = t83 * t82;
        float t86 = corners[5] * t58;
        float t87 = t72 * t86;
        float t89 = corners[7] * t53;
        float t92 = t72 * t59;
        float t93 = t60 * corners[2];
        float t94 = corners[1] * t74;
        float t95 = t94 * t93;
        float t97 = corners[3] * t74;
        float t98 = t79 * t97;
        float t99 = corners[1] * t86;
        float t101 = 0.2e1f * t99 * t48;
        float t103 = t44 * t25;
        float t104 = corners[2] * corners[3];
        float t107 = corners[5] * corners[4];
        float t108 = t74 * t16;
        float t110 = t71 * t44;
        float t111 = t104 * t110;
        float t112 = t25 * corners[0];
        float t113 = corners[2] * corners[4];
        float t117 = 0.2e1f * t80 + 0.2e1f * t84 - 0.2e1f * t87 + 0.2e1f * t89 * t67 + t92 - 0.2e1f * t95 - t98 + t101 + t86 * t26 - t104 * t103 + t97 * t93 - t108 * t107 - t111 - 0.2e1f * corners[5] * t113 * t112;
        float t119 = corners[2] * t30;
        float t120 = corners[3] * t25;
        float t121 = t120 * t119;
        float t122 = t86 * t76;
        float t123 = corners[2] * t48;
        float t126 = t60 * t30;
        float t127 = t104 * t126;
        float t128 = corners[1] * t9;
        float t129 = t128 * t123;
        float t131 = corners[6] * corners[7];
        float t133 = corners[1] * t15;
        float t134 = corners[3] * t6;
        float t138 = t25 * t30;
        float t139 = t3 * t138;
        float t141 = corners[5] * t30;
        float t142 = t20 * t141;
        float t143 = t20 * t31;
        float t147 = corners[6] * t30;
        float t148 = corners[7] * t16;
        float t149 = t148 * t147;
        float t150 = corners[1] * t58;
        float t151 = t150 * t67;
        float t152 = t121 + t122 - 0.2e1f * t55 * t123 + t127 - 0.2e1f * t129 - t131 * t126 + 0.2e1f * t134 * t133 + t94 * t67 - 0.2e1f * t139 - t142 + 0.2e1f * t143 - 0.2e1f * t50 * t45 - t149 - t151;
        float t154 = t1 * t103;
        float t157 = corners[1] * t25;
        float t158 = t157 * t58 * corners[0];
        float t159 = corners[1] * t44;
        float t161 = t16 * t44;
        float t162 = t161 * t48;
        float t164 = t44 * corners[0];
        float t165 = t16 * corners[1];
        float t166 = t165 * t164;
        float t167 = t74 * corners[0];
        float t168 = t165 * t167;
        float t169 = t71 * t36;
        float t170 = t169 * t8;
        float t173 = corners[3] * t2 * t94;
        float t176 = 0.2e1f * t134 * t31;
        float t177 = corners[1] * t75;
        float t180 = t71 * t53;
        float t182 = 0.2e1f * t180 * t3;
        float t183 = corners[1] * t104;
        float t184 = t183 * t45;
        float t186 = t99 * t11;
        float t188 = t141 * t26 + 0.2e1f * t154 - t158 - t159 * t112 - 0.2e1f * t162 + t166 + t168 - 0.2e1f * t170 - 0.2e1f * t173 - t176 + 0.2e1f * t177 * t3 + t182 + 0.2e1f * t184 - 0.2e1f * t186;

        float det  = t78 + t117 + t152 + t188;

        float t191 = 0.1e1f / det;
        float t194 = corners[3] * t15;
        float t195 = corners[0] * corners[4];
        float t196 = corners[3] * t195;
        float t198 = corners[1] * t53;
        float t200 = corners[3] * t36;
        float t202 = corners[3] * t30;
        float t203 = -t194 - t196 + corners[1] * t195 + t198 + t82 - corners[1] * t24 - t54 + t200 - t37 + corners[5] * t15 - t141 + t202;
        float t208 = t3 * t48;
        float t209 = corners[7] * corners[1];
        float t212 = corners[7] * corners[5];
        float t213 = t212 * t24;
        float t214 = corners[1] * corners[0];
        float t216 = t64 * t6;
        float t217 = t64 * t3;
        float t218 = corners[1] * corners[5];
        float t220 = corners[7] * t202 - corners[5] * t31 - t208 - t209 * t24 + t5 * t48 + t213 - t10 * t214 + t180 - t216 + t217 - t169 + t218 * t15;
        float t223 = corners[3] * t45;
        float t225 = t74 * t60;
        float t232 = t25 * corners[2];
        float t236 = -t223 + t103 - corners[2] * t61 + t225 - corners[6] * t67 + corners[1] * t97 + t208 + corners[3] * t159 - t177 - corners[3] * t75 - corners[7] * t159 + corners[2] * t67 - corners[4] * t232 + t25 * t24 - corners[4] * t112 + t216;
        float t249 = corners[3] * corners[7];
        float t256 = -0.2e1f * t213 - 0.2e1f * t217 - 0.2e1f * t36 * t212 + t3 * t9 - t3 * t2 + t11 * t6 + t9 * t5 - t6 * t5 + t8 * t5 + t64 * t11 + t131 * t2 - t249 * t15 + t131 * t3 - t64 * t8 + t9 * t1 + t131 * t6 + t11 * t2;
        float t261 = t200 * t6;
        float t264 = corners[4] * t138;
        float t267 = 0.2e1f * t198 * t3;
        float t268 = t25 * t119;
        float t269 = corners[2] * t126;
        float t270 = corners[6] * t126;
        float t272 = corners[3] * corners[1];
        float t274 = t74 * t93;
        float t275 = t37 * t48;
        float t277 = -t261 - t86 * t11 + t128 * t195 - t264 - t89 * t3 + t267 + t268 + t269 - t270 + t133 * t6 + t272 * t167 - t274 + t275 + t3 * t31;
        float t279 = corners[3] * t113 * t2;
        float t282 = 0.2e1f * t133 * t3;
        float t283 = t25 * t58;
        float t284 = corners[0] * t283;
        float t286 = 0.2e1f * t212 * t119;
        float t289 = corners[1] * t131 * t24;
        float t294 = t11 * t141;
        float t295 = corners[5] * t4;
        float t298 = 0.2e1f * t2 * t59;
        float t299 = t37 * t8;
        float t300 = t74 * t67;
        float t301 = t279 - t249 * t147 - t282 - t284 - t286 - t64 * t86 - t289 - t94 * t2 - t196 * t8 + corners[7] * t1 * t53 + t294 - t295 * t195 + t298 - t299 + t300;
        float t303 = corners[4] * t283;
        float t304 = t58 * t67;
        float t305 = t44 * t112;
        float t306 = corners[7] * t4;
        float t307 = t306 * t195;
        float t308 = corners[6] * t63;
        float t310 = t4 * t97;
        float t311 = t131 * t86;
        float t313 = corners[2] * t103;
        float t314 = t3 * t141;
        float t315 = t10 * t141;
        float t316 = t272 * t164;
        float t319 = t303 - t304 + t305 + t307 + t308 - t295 * t36 - t310 - t311 + t41 * t195 - t313 - t314 + t315 + t316 - corners[3] * t44 * t48;
        float t320 = t209 * t164;
        float t321 = t97 * t2;
        float t323 = t200 * t2;
        float t325 = t8 * t159;
        float t327 = t59 * t5;
        float t328 = t104 * t75;
        float t330 = t64 * t59;
        float t332 = t68 * t2;
        float t333 = corners[6] * t11;
        float t334 = t333 * t6;
        float t336 = t333 * t2;
        float t338 = -t320 - t321 - t104 * t159 + t323 + t86 * t5 + t325 + t104 * t45 - t327 + t328 + t131 * t141 + t330 + t94 * t6 - t332 + 0.2e1f * t334 - 0.2e1f * t336;
        float t346 = corners[2] * t44;
        float t349 = -t19 + t22 + t28 + t33 + t39 - t43 - t59 * t76 - corners[1] * t2 * corners[2] * t11 - t346 * t157 + t51 - t56 + t65 - t69 - t54 * t76;
        float t350 = corners[0] * t16;
        float t356 = t80 + corners[6] * t350 * t11 + t84 - t87 + t194 * t35 + t92 - t95 - t350 * t75 + t83 * t141 - t98 + t101 - t111 + t121 + t122 + t127;
        float t361 = -t129 - t139 - t142 + t143 - t149 + t26 * t150 - t151 + t154 - t158 - t162 + t166 + t168 + t196 * t128 - t223 * t214;
        float t368 = t30 * t16;
        float t372 = t133 * t93 - t170 - t173 - t176 - t128 * t59 + t182 - t198 * t83 + t184 - t196 * t232 - t10 * t126 + t97 * t67 - t186 + t368 * t9 + t8 * t110 + t295 * t97;
        float t394 = t16 * t75 - corners[1] * t63 - corners[5] * t283 + 0.2e1f * corners[1] * t107 * t8 + corners[5] * t368 + corners[1] * t283 - t71 * t45 + corners[7] * t126 + t71 * t97 - t71 * t75 - t30 * corners[5] * t25 + t60 * t59 + 0.2e1f * t55 * t6 + t71 * t86;
        float t419 = -0.2e1f * t11 * t46 - t71 * t59 - 0.2e1f * corners[3] * t107 * t8 + t16 * t45 + corners[3] * t110 - t16 * t159 - corners[7] * t368 - 0.2e1f * t5 * t232 + 0.2e1f * t64 * t20 + 0.2e1f * corners[1] * t11 * t10 - 0.2e1f * t183 * t11 + corners[1] * t225 + 0.2e1f * t32 * t11 + 0.2e1f * t76 * t8;
        float t445 = corners[3] * t218;
        float t448 = corners[3] * t209;
        float t451 = 0.2e1f * t232 * t2 + 0.2e1f * corners[1] * t60 * t24 - 0.2e1f * t6 * t76 - 0.2e1f * t46 * t2 - 0.2e1f * t120 * t24 - 0.2e1f * t60 * corners[7] * t24 - 0.2e1f * t64 * t67 + 0.2e1f * t148 * t15 + 0.2e1f * t10 * t67 - corners[3] * t103 + corners[1] * t103 - 0.2e1f * t306 * t2 - 0.2e1f * t445 * t24 + 0.2e1f * t448 * t24;
        float t452 = corners[3] * t212;
        float t477 = 0.2e1f * t452 * t24 - 0.2e1f * t452 * t15 - 0.2e1f * t448 * t15 - 0.2e1f * t72 * t3 - 0.2e1f * t37 * t212 - 0.2e1f * t55 * t9 - corners[1] * t108 - corners[3] * t126 + 0.2e1f * t76 * t9 + 0.2e1f * t3 * t232 + 0.2e1f * t128 * t48 + 0.2e1f * t445 * t15 - corners[3] * t225 + corners[3] * t138;
        float t494 = -corners[4] * t71 * t74 - t16 * t164 - 0.2e1f * t261 + 0.2e1f * t50 * t24 - corners[4] * t108 - t264 + t16 * t167 + corners[4] * t71 * t58 - 0.2e1f * corners[4] * t212 * t24 + t267 + t268 + t269 - t270 - t274;
        float t512 = t71 * t44 * corners[6] + 0.2e1f * t275 + 0.2e1f * corners[7] * t15 * t6 + 0.2e1f * t279 - t282 - t284 - t71 * t346 - t286 - t71 * corners[6] * t58 - 0.2e1f * t289 - t16 * t147 + t71 * t74 * corners[2] + 0.2e1f * t294 + t16 * corners[4] * t30;
        float t521 = t298 - 0.2e1f * t299 + t300 + t303 - t304 + t305 + 0.2e1f * t307 + t308 + corners[6] * t161 - 0.2e1f * t310 - 0.2e1f * t311 - t313 - 0.2e1f * t314 + 0.2e1f * t315;
        float t525 = t316 - t320 - t321 + t97 * t5 + t323 + t325 - t327 + t328 + t330 - t332 + t334 - t336 - t10 * t159 - t183 * t195;

        t[0] = -t191 * (-corners[7] + corners[1]) * t12 * (t1 - t2 - t3 - t4 + t5 + t6);
        t[3] = t191 * t203 * t12;
        t[6] = -t191 * t220 * t12;
        t[1] = t191 * (-corners[3] + corners[1]) * (t236 + t256);
        t[4] = -t191 * (t277 + t301 + t319 + t338);
        t[7] = t191 * (t349 + t356 + t361 + t372);
        t[2] = t191 * (t394 + t419 + t451 + t477);
        t[5] = -t191 * (t494 + t512 + t521 + 0.2e1f * t525);
        t[8] = 1;

        return det;
}

/**
 * x[TL, TR, BR, BL] -> unit square[(0,0) - (1,0) - (1,1) - (0,1)]
 *
 * @param t 3x3 transformation matrix
 * @param corners Corner coordinates of a deformed unit-square, [x, y, x, y, x, y, x, y]
 *
 * @return determinant of the computation
 */
double nx_transform_2d_fill_perspective_d(double *t, const double *corners)
{
        double t1 = corners[0] * corners[3];
        double t2 = corners[0] * corners[5];
        double t3 = corners[4] * corners[3];
        double t4 = corners[2] * corners[1];
        double t5 = corners[4] * corners[1];
        double t6 = corners[2] * corners[5];
        double t8 = corners[2] * corners[7];
        double t9 = corners[6] * corners[5];
        double t10 = corners[6] * corners[3];
        double t11 = corners[7] * corners[4];
        double t12 = -t8 + t6 - t3 - t9 + t10 + t11;
        double t15 = corners[0] * corners[6];
        double t16 = corners[3]*corners[3];
        double t19 = 0.2e1 * t16 * t5 * t15;
        double t20 = t16 * corners[4];
        double t22 = corners[6] * t20 * t2;
        double t24 = corners[0] * corners[2];
        double t25 = corners[7]*corners[7];
        double t26 = corners[4] * t25;
        double t28 = corners[1] * t26 * t24;
        double t30 = corners[0]*corners[0];
        double t31 = corners[7] * t30;
        double t32 = corners[3] * t9;
        double t33 = t32 * t31;
        double t35 = corners[5] * t8;
        double t36 = corners[6] * corners[4];
        double t37 = corners[1] * t36;
        double t39 = 0.2e1 * t37 * t35;
        double t41 = corners[7] * t10;
        double t43 = 0.2e1 * t41 * corners[4] * t2;
        double t44 = corners[4]*corners[4];
        double t45 = t44 * corners[7];
        double t46 = corners[6] * t16;
        double t48 = corners[7] * corners[0];
        double t50 = corners[1] * t10;
        double t51 = t50 * corners[4] * t48;
        double t53 = corners[6] * corners[2];
        double t54 = corners[5] * t53;
        double t55 = corners[3] * t5;
        double t56 = t55 * t54;
        double t58 = corners[2]*corners[2];
        double t59 = corners[7] * t58;
        double t60 = corners[5]*corners[5];
        double t61 = corners[6] * t60;
        double t63 = t60 * t58;
        double t64 = corners[6] * corners[1];
        double t65 = t64 * t63;
        double t67 = t60 * corners[0];
        double t68 = corners[3] * t53;
        double t69 = t68 * t67;
        double t71 = corners[1]*corners[1];
        double t72 = t71 * corners[6];
        double t74 = corners[6]*corners[6];
        double t75 = t74 * corners[5];
        double t76 = corners[4] * t71;
        double t78 = -t19 + 0.2e1 * t22 + 0.2e1 * t28 + 0.2e1 * t33 + t39 - t43 + t46 * t45 + 0.2e1 * t51 - 0.2e1 * t56 - t61 * t59 + 0.2e1 * t65 - 0.2e1 * t69 + t72 * t45 - t76 * t75;
        double t79 = corners[2] * t71;
        double t80 = t79 * t75;
        double t82 = corners[5] * t24;
        double t83 = corners[7] * t3;
        double t84 = t83 * t82;
        double t86 = corners[5] * t58;
        double t87 = t72 * t86;
        double t89 = corners[7] * t53;
        double t92 = t72 * t59;
        double t93 = t60 * corners[2];
        double t94 = corners[1] * t74;
        double t95 = t94 * t93;
        double t97 = corners[3] * t74;
        double t98 = t79 * t97;
        double t99 = corners[1] * t86;
        double t101 = 0.2e1 * t99 * t48;
        double t103 = t44 * t25;
        double t104 = corners[2] * corners[3];
        double t107 = corners[5] * corners[4];
        double t108 = t74 * t16;
        double t110 = t71 * t44;
        double t111 = t104 * t110;
        double t112 = t25 * corners[0];
        double t113 = corners[2] * corners[4];
        double t117 = 0.2e1 * t80 + 0.2e1 * t84 - 0.2e1 * t87 + 0.2e1 * t89 * t67 + t92 - 0.2e1 * t95 - t98 + t101 + t86 * t26 - t104 * t103 + t97 * t93 - t108 * t107 - t111 - 0.2e1 * corners[5] * t113 * t112;
        double t119 = corners[2] * t30;
        double t120 = corners[3] * t25;
        double t121 = t120 * t119;
        double t122 = t86 * t76;
        double t123 = corners[2] * t48;
        double t126 = t60 * t30;
        double t127 = t104 * t126;
        double t128 = corners[1] * t9;
        double t129 = t128 * t123;
        double t131 = corners[6] * corners[7];
        double t133 = corners[1] * t15;
        double t134 = corners[3] * t6;
        double t138 = t25 * t30;
        double t139 = t3 * t138;
        double t141 = corners[5] * t30;
        double t142 = t20 * t141;
        double t143 = t20 * t31;
        double t147 = corners[6] * t30;
        double t148 = corners[7] * t16;
        double t149 = t148 * t147;
        double t150 = corners[1] * t58;
        double t151 = t150 * t67;
        double t152 = t121 + t122 - 0.2e1 * t55 * t123 + t127 - 0.2e1 * t129 - t131 * t126 + 0.2e1 * t134 * t133 + t94 * t67 - 0.2e1 * t139 - t142 + 0.2e1 * t143 - 0.2e1 * t50 * t45 - t149 - t151;
        double t154 = t1 * t103;
        double t157 = corners[1] * t25;
        double t158 = t157 * t58 * corners[0];
        double t159 = corners[1] * t44;
        double t161 = t16 * t44;
        double t162 = t161 * t48;
        double t164 = t44 * corners[0];
        double t165 = t16 * corners[1];
        double t166 = t165 * t164;
        double t167 = t74 * corners[0];
        double t168 = t165 * t167;
        double t169 = t71 * t36;
        double t170 = t169 * t8;
        double t173 = corners[3] * t2 * t94;
        double t176 = 0.2e1 * t134 * t31;
        double t177 = corners[1] * t75;
        double t180 = t71 * t53;
        double t182 = 0.2e1 * t180 * t3;
        double t183 = corners[1] * t104;
        double t184 = t183 * t45;
        double t186 = t99 * t11;
        double t188 = t141 * t26 + 0.2e1 * t154 - t158 - t159 * t112 - 0.2e1 * t162 + t166 + t168 - 0.2e1 * t170 - 0.2e1 * t173 - t176 + 0.2e1 * t177 * t3 + t182 + 0.2e1 * t184 - 0.2e1 * t186;

        double det  = t78 + t117 + t152 + t188;

        double t191 = 0.1e1 / det;
        double t194 = corners[3] * t15;
        double t195 = corners[0] * corners[4];
        double t196 = corners[3] * t195;
        double t198 = corners[1] * t53;
        double t200 = corners[3] * t36;
        double t202 = corners[3] * t30;
        double t203 = -t194 - t196 + corners[1] * t195 + t198 + t82 - corners[1] * t24 - t54 + t200 - t37 + corners[5] * t15 - t141 + t202;
        double t208 = t3 * t48;
        double t209 = corners[7] * corners[1];
        double t212 = corners[7] * corners[5];
        double t213 = t212 * t24;
        double t214 = corners[1] * corners[0];
        double t216 = t64 * t6;
        double t217 = t64 * t3;
        double t218 = corners[1] * corners[5];
        double t220 = corners[7] * t202 - corners[5] * t31 - t208 - t209 * t24 + t5 * t48 + t213 - t10 * t214 + t180 - t216 + t217 - t169 + t218 * t15;
        double t223 = corners[3] * t45;
        double t225 = t74 * t60;
        double t232 = t25 * corners[2];
        double t236 = -t223 + t103 - corners[2] * t61 + t225 - corners[6] * t67 + corners[1] * t97 + t208 + corners[3] * t159 - t177 - corners[3] * t75 - corners[7] * t159 + corners[2] * t67 - corners[4] * t232 + t25 * t24 - corners[4] * t112 + t216;
        double t249 = corners[3] * corners[7];
        double t256 = -0.2e1 * t213 - 0.2e1 * t217 - 0.2e1 * t36 * t212 + t3 * t9 - t3 * t2 + t11 * t6 + t9 * t5 - t6 * t5 + t8 * t5 + t64 * t11 + t131 * t2 - t249 * t15 + t131 * t3 - t64 * t8 + t9 * t1 + t131 * t6 + t11 * t2;
        double t261 = t200 * t6;
        double t264 = corners[4] * t138;
        double t267 = 0.2e1 * t198 * t3;
        double t268 = t25 * t119;
        double t269 = corners[2] * t126;
        double t270 = corners[6] * t126;
        double t272 = corners[3] * corners[1];
        double t274 = t74 * t93;
        double t275 = t37 * t48;
        double t277 = -t261 - t86 * t11 + t128 * t195 - t264 - t89 * t3 + t267 + t268 + t269 - t270 + t133 * t6 + t272 * t167 - t274 + t275 + t3 * t31;
        double t279 = corners[3] * t113 * t2;
        double t282 = 0.2e1 * t133 * t3;
        double t283 = t25 * t58;
        double t284 = corners[0] * t283;
        double t286 = 0.2e1 * t212 * t119;
        double t289 = corners[1] * t131 * t24;
        double t294 = t11 * t141;
        double t295 = corners[5] * t4;
        double t298 = 0.2e1 * t2 * t59;
        double t299 = t37 * t8;
        double t300 = t74 * t67;
        double t301 = t279 - t249 * t147 - t282 - t284 - t286 - t64 * t86 - t289 - t94 * t2 - t196 * t8 + corners[7] * t1 * t53 + t294 - t295 * t195 + t298 - t299 + t300;
        double t303 = corners[4] * t283;
        double t304 = t58 * t67;
        double t305 = t44 * t112;
        double t306 = corners[7] * t4;
        double t307 = t306 * t195;
        double t308 = corners[6] * t63;
        double t310 = t4 * t97;
        double t311 = t131 * t86;
        double t313 = corners[2] * t103;
        double t314 = t3 * t141;
        double t315 = t10 * t141;
        double t316 = t272 * t164;
        double t319 = t303 - t304 + t305 + t307 + t308 - t295 * t36 - t310 - t311 + t41 * t195 - t313 - t314 + t315 + t316 - corners[3] * t44 * t48;
        double t320 = t209 * t164;
        double t321 = t97 * t2;
        double t323 = t200 * t2;
        double t325 = t8 * t159;
        double t327 = t59 * t5;
        double t328 = t104 * t75;
        double t330 = t64 * t59;
        double t332 = t68 * t2;
        double t333 = corners[6] * t11;
        double t334 = t333 * t6;
        double t336 = t333 * t2;
        double t338 = -t320 - t321 - t104 * t159 + t323 + t86 * t5 + t325 + t104 * t45 - t327 + t328 + t131 * t141 + t330 + t94 * t6 - t332 + 0.2e1 * t334 - 0.2e1 * t336;
        double t346 = corners[2] * t44;
        double t349 = -t19 + t22 + t28 + t33 + t39 - t43 - t59 * t76 - corners[1] * t2 * corners[2] * t11 - t346 * t157 + t51 - t56 + t65 - t69 - t54 * t76;
        double t350 = corners[0] * t16;
        double t356 = t80 + corners[6] * t350 * t11 + t84 - t87 + t194 * t35 + t92 - t95 - t350 * t75 + t83 * t141 - t98 + t101 - t111 + t121 + t122 + t127;
        double t361 = -t129 - t139 - t142 + t143 - t149 + t26 * t150 - t151 + t154 - t158 - t162 + t166 + t168 + t196 * t128 - t223 * t214;
        double t368 = t30 * t16;
        double t372 = t133 * t93 - t170 - t173 - t176 - t128 * t59 + t182 - t198 * t83 + t184 - t196 * t232 - t10 * t126 + t97 * t67 - t186 + t368 * t9 + t8 * t110 + t295 * t97;
        double t394 = t16 * t75 - corners[1] * t63 - corners[5] * t283 + 0.2e1 * corners[1] * t107 * t8 + corners[5] * t368 + corners[1] * t283 - t71 * t45 + corners[7] * t126 + t71 * t97 - t71 * t75 - t30 * corners[5] * t25 + t60 * t59 + 0.2e1 * t55 * t6 + t71 * t86;
        double t419 = -0.2e1 * t11 * t46 - t71 * t59 - 0.2e1 * corners[3] * t107 * t8 + t16 * t45 + corners[3] * t110 - t16 * t159 - corners[7] * t368 - 0.2e1 * t5 * t232 + 0.2e1 * t64 * t20 + 0.2e1 * corners[1] * t11 * t10 - 0.2e1 * t183 * t11 + corners[1] * t225 + 0.2e1 * t32 * t11 + 0.2e1 * t76 * t8;
        double t445 = corners[3] * t218;
        double t448 = corners[3] * t209;
        double t451 = 0.2e1 * t232 * t2 + 0.2e1 * corners[1] * t60 * t24 - 0.2e1 * t6 * t76 - 0.2e1 * t46 * t2 - 0.2e1 * t120 * t24 - 0.2e1 * t60 * corners[7] * t24 - 0.2e1 * t64 * t67 + 0.2e1 * t148 * t15 + 0.2e1 * t10 * t67 - corners[3] * t103 + corners[1] * t103 - 0.2e1 * t306 * t2 - 0.2e1 * t445 * t24 + 0.2e1 * t448 * t24;
        double t452 = corners[3] * t212;
        double t477 = 0.2e1 * t452 * t24 - 0.2e1 * t452 * t15 - 0.2e1 * t448 * t15 - 0.2e1 * t72 * t3 - 0.2e1 * t37 * t212 - 0.2e1 * t55 * t9 - corners[1] * t108 - corners[3] * t126 + 0.2e1 * t76 * t9 + 0.2e1 * t3 * t232 + 0.2e1 * t128 * t48 + 0.2e1 * t445 * t15 - corners[3] * t225 + corners[3] * t138;
        double t494 = -corners[4] * t71 * t74 - t16 * t164 - 0.2e1 * t261 + 0.2e1 * t50 * t24 - corners[4] * t108 - t264 + t16 * t167 + corners[4] * t71 * t58 - 0.2e1 * corners[4] * t212 * t24 + t267 + t268 + t269 - t270 - t274;
        double t512 = t71 * t44 * corners[6] + 0.2e1 * t275 + 0.2e1 * corners[7] * t15 * t6 + 0.2e1 * t279 - t282 - t284 - t71 * t346 - t286 - t71 * corners[6] * t58 - 0.2e1 * t289 - t16 * t147 + t71 * t74 * corners[2] + 0.2e1 * t294 + t16 * corners[4] * t30;
        double t521 = t298 - 0.2e1 * t299 + t300 + t303 - t304 + t305 + 0.2e1 * t307 + t308 + corners[6] * t161 - 0.2e1 * t310 - 0.2e1 * t311 - t313 - 0.2e1 * t314 + 0.2e1 * t315;
        double t525 = t316 - t320 - t321 + t97 * t5 + t323 + t325 - t327 + t328 + t330 - t332 + t334 - t336 - t10 * t159 - t183 * t195;

        t[0] = -t191 * (-corners[7] + corners[1]) * t12 * (t1 - t2 - t3 - t4 + t5 + t6);
        t[3] = t191 * t203 * t12;
        t[6] = -t191 * t220 * t12;
        t[1] = t191 * (-corners[3] + corners[1]) * (t236 + t256);
        t[4] = -t191 * (t277 + t301 + t319 + t338);
        t[7] = t191 * (t349 + t356 + t361 + t372);
        t[2] = t191 * (t394 + t419 + t451 + t477);
        t[5] = -t191 * (t494 + t512 + t521 + 0.2e1 * t525);
        t[8] = 1;

        return det;
}

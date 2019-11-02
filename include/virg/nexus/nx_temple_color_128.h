/**
 * @file nx_temple_color_128.h
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
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
 * You should have received a copy of the GNU General Public License
 * along with VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_TEMPLE_COLOR_128_H
#define VIRG_NEXUS_NX_TEMPLE_COLOR_128_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_image.h"

__NX_BEGIN_DECL

#define NX_TEMPLE_COLOR_128_N_SEQ 129

static const char *NX_TEMPLE_COLOR_128_SEQ_NAMES[NX_TEMPLE_COLOR_128_N_SEQ] = {
        "Airport_ce", "Baby_ce", "Badminton_ce1", "Badminton_ce2",
        "Ball_ce1", "Ball_ce2", "Ball_ce3", "Ball_ce4",
        "Basketball", "Basketball_ce1", "Basketball_ce2", "Basketball_ce3",
        "Bee_ce", "Bicycle", "Bike_ce1", "Bike_ce2", "Biker", "Bikeshow_ce",
        "Bird", "Board", "Boat_ce1", "Boat_ce2", "Bolt", "Boy",
        "Busstation_ce1", "Busstation_ce2",
        "Carchasing_ce1", "Carchasing_ce3", "Carchasing_ce4",
        "CarDark", "CarScale", "Charger_ce", "Coke", "Couple", "Crossing",
        "Cup", "Cup_ce", "David", "David3", "Deer", "Diving", "Doll",
        "Eagle_ce", "Electricalbike_ce", "Face_ce", "Face_ce2", "FaceOcc1",
        "Fish_ce1", "Fish_ce2", "Football1", "Girl", "Girlmov",
        "Guitar_ce1", "Guitar_ce2", "Gym", "Hand", "Hand_ce1", "Hand_ce2",
        "Hurdle_ce1", "Hurdle_ce2", "Iceskater", "Ironman",
        "Jogging1", "Jogging2", "Juice", "Kite_ce1", "Kite_ce2", "Kite_ce3",
        "Kobe_ce", "Lemming", "Liquor", "Logo_ce", "Matrix", "Messi_ce",
        "Michaeljackson_ce", "Microphone_ce1", "Microphone_ce2", "Motorbike_ce",
        "MotorRolling", "MountainBike", "Panda", "Plane_ce2",
        "Plate_ce1", "Plate_ce2", "Pool_ce1", "Pool_ce2", "Pool_ce3",
        "Railwaystation_ce", "Ring_ce", "Sailor_ce", "Shaking",
        "Singer1", "Singer2", "Singer_ce1", "Singer_ce2",
        "Skating1", "Skating2", "Skating_ce1", "Skating_ce2",
        "Skiing", "Skiing_ce", "Skyjumping_ce", "Soccer", "Spiderman_ce",
        "Subway", "Suitcase_ce", "Sunshade", "SuperMario_ce",
        "Surf_ce1", "Surf_ce2", "Surf_ce3", "Surf_ce4",
        "TableTennis_ce", "TennisBall_ce",
        "Tennis_ce1", "Tennis_ce2", "Tennis_ce3", "Thunder_ce",
        "Tiger1", "Tiger2", "Torus", "Toyplane_ce", "Trellis",
        "Walking", "Walking2", "Woman",
        "Yo-yos_ce1", "Yo-yos_ce2", "Yo-yos_ce3" };

#define NX_TEMPLE_COLOR_128_ATTRIB_VI   0x0001 // Illumination Variation
#define NX_TEMPLE_COLOR_128_ATTRIB_SV   0x0002 // Scale Variation
#define NX_TEMPLE_COLOR_128_ATTRIB_OCC  0x0004 // OCClusion
#define NX_TEMPLE_COLOR_128_ATTRIB_DEF  0x0008 // DEFormation
#define NX_TEMPLE_COLOR_128_ATTRIB_MB   0x0010 // Motion Blur
#define NX_TEMPLE_COLOR_128_ATTRIB_FM   0x0020 // Fast Motion
#define NX_TEMPLE_COLOR_128_ATTRIB_IPR  0x0040 // In Plane Rotation
#define NX_TEMPLE_COLOR_128_ATTRIB_OPR  0x0080 // Out of Plane Rotation
#define NX_TEMPLE_COLOR_128_ATTRIB_OV   0x0100 // Out-of-View
#define NX_TEMPLE_COLOR_128_ATTRIB_BC   0x0200 // Background Clutter
#define NX_TEMPLE_COLOR_128_ATTRIB_LR   0x0400 // Low Resolution

struct NXTempleColor128Sequence {
        char *base_dir;
        char *name;
        int length;
        int start_frame;
        int end_frame;
        uint16_t attributes;
        float **ground_truth;
};

struct NXTempleColor128Sequence *nx_temple_color_128_sequence_new(const char *base_dir,
                                                                  const char *seq_name);

void nx_temple_color_128_sequence_free(struct NXTempleColor128Sequence *seq);

void nx_temple_color_128_sequence_print(struct NXTempleColor128Sequence *seq);

void nx_temple_color_128_sequence_xload_frame(const struct NXTempleColor128Sequence *seq,
                                              int frame_id,
                                              struct NXImage *img);


__NX_END_DECL

#endif

/*
 * Modified for use with MPlayer, for details see the changelog at
 * http://svn.mplayerhq.hu/mplayer/trunk/
 * $Id: l2tables.h 30209 2010-01-04 19:36:18Z diego $
 */

#ifndef MPLAYER_MP3LIB_L2TABLES_H
#define MPLAYER_MP3LIB_L2TABLES_H

#include "mpg123.h"

/*
 * Layer 2 Alloc tables ..
 * most other tables are calculated on program start (which is (of course)
 * not ISO-conform) ..
 * Layer-3 huffman table is in huffman.h
 */

static struct al_table alloc_0[] = {
	{4,0},{5,3},{3,-3},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},{9,-255},{10,-511},
	{11,-1023},{12,-2047},{13,-4095},{14,-8191},{15,-16383},{16,-32767},
	{4,0},{5,3},{3,-3},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},{9,-255},{10,-511},
	{11,-1023},{12,-2047},{13,-4095},{14,-8191},{15,-16383},{16,-32767},
	{4,0},{5,3},{3,-3},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},{9,-255},{10,-511},
	{11,-1023},{12,-2047},{13,-4095},{14,-8191},{15,-16383},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{2,0},{5,3},{7,5},{16,-32767},
	{2,0},{5,3},{7,5},{16,-32767},
	{2,0},{5,3},{7,5},{16,-32767},
	{2,0},{5,3},{7,5},{16,-32767} };

static struct al_table alloc_1[] = {
	{4,0},{5,3},{3,-3},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},{9,-255},{10,-511},
	{11,-1023},{12,-2047},{13,-4095},{14,-8191},{15,-16383},{16,-32767},
	{4,0},{5,3},{3,-3},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},{9,-255},{10,-511},
	{11,-1023},{12,-2047},{13,-4095},{14,-8191},{15,-16383},{16,-32767},
	{4,0},{5,3},{3,-3},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},{9,-255},{10,-511},
	{11,-1023},{12,-2047},{13,-4095},{14,-8191},{15,-16383},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
	{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{3,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{16,-32767},
	{2,0},{5,3},{7,5},{16,-32767},
	{2,0},{5,3},{7,5},{16,-32767},
	{2,0},{5,3},{7,5},{16,-32767},
	{2,0},{5,3},{7,5},{16,-32767},
	{2,0},{5,3},{7,5},{16,-32767},
	{2,0},{5,3},{7,5},{16,-32767},
	{2,0},{5,3},{7,5},{16,-32767} };

static struct al_table alloc_2[] = {
	{4,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},{9,-255},
	{10,-511},{11,-1023},{12,-2047},{13,-4095},{14,-8191},{15,-16383},
	{4,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},{9,-255},
	{10,-511},{11,-1023},{12,-2047},{13,-4095},{14,-8191},{15,-16383},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63} };

static struct al_table alloc_3[] = {
	{4,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},{9,-255},
	{10,-511},{11,-1023},{12,-2047},{13,-4095},{14,-8191},{15,-16383},
	{4,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},{9,-255},
	{10,-511},{11,-1023},{12,-2047},{13,-4095},{14,-8191},{15,-16383},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63} };

static struct al_table alloc_4[] = {
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
		{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{14,-8191},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
		{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{14,-8191},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
		{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{14,-8191},
	{4,0},{5,3},{7,5},{3,-3},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},{8,-127},
		{9,-255},{10,-511},{11,-1023},{12,-2047},{13,-4095},{14,-8191},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{3,0},{5,3},{7,5},{10,9},{4,-7},{5,-15},{6,-31},{7,-63},
	{2,0},{5,3},{7,5},{10,9},
	{2,0},{5,3},{7,5},{10,9},
	{2,0},{5,3},{7,5},{10,9},
	{2,0},{5,3},{7,5},{10,9},
	{2,0},{5,3},{7,5},{10,9},
	{2,0},{5,3},{7,5},{10,9},
	{2,0},{5,3},{7,5},{10,9},
	{2,0},{5,3},{7,5},{10,9},
	{2,0},{5,3},{7,5},{10,9},
	{2,0},{5,3},{7,5},{10,9},
	{2,0},{5,3},{7,5},{10,9},
    {2,0},{5,3},{7,5},{10,9},
    {2,0},{5,3},{7,5},{10,9},
    {2,0},{5,3},{7,5},{10,9},
    {2,0},{5,3},{7,5},{10,9},
    {2,0},{5,3},{7,5},{10,9},
    {2,0},{5,3},{7,5},{10,9},
    {2,0},{5,3},{7,5},{10,9},
    {2,0},{5,3},{7,5},{10,9}  };

#endif /* MPLAYER_MP3LIB_L2TABLES_H */

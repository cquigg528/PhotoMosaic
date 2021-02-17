/**
 * @file rgbtree.cpp
 * Implementation of rgbtree class.
 */

#include <utility>
#include <algorithm>
#include "rgbtree.h"

using namespace std;

rgbtree::rgbtree(const map<RGBAPixel,string>& photos)
{
  // extract keys into tree
  for (auto const& pix: photos) {
    tree.push_back(pix.first);
  }

  int median = (tree.size() - 1) / 2;

  build(0, tree.size()-1, median, 0);
}

void rgbtree::build(int start, int end, int median, int level) {
  if (start < end) {
    int dim = level % 3;
    quickSelect(start, end, median, dim);
    
    // recurse over L, R subtrees
    build(start, median-1, (start + median-1)/2, level+1);
    build(median+1, end, (median+1+end)/2, level+1);
  }

}

RGBAPixel rgbtree::findNearestNeighbor(const RGBAPixel & query) const
{
  int end = tree.size() - 1;
  int currmin = 100000;
  RGBAPixel q;
  return workDown(query, 0, end, currmin, q, 0);
}

bool rgbtree::smallerByDim(const RGBAPixel & first,
                                const RGBAPixel & second, int curDim) const
{
  int first_val = getDim(first, curDim);
  int second_val = getDim(second, curDim);
  if (first_val == second_val) {
    return first < second;
  } else {
    return first_val < second_val;
  }
}

/**
 * This function splits the trees[start..end] subarray at position start k
 */
void rgbtree::quickSelect(int start, int end, int k, int d)
{
  // if lo >= hi, tree[k] is median
  if (start < end) {
    int p = partition(start, end, d);
    if (k < p) {
      quickSelect(start, p-1, k, d);
    } else if (k > p) {
      quickSelect(p+1, end, k, d);
    }
  }
}


/**
 * This method does a partition around pivot and will be used 
 * in quick select. It uses tree[lo] as the default pivot.
 * It returns the index of the pivot in the updated vector.
 * You will likely need to modify and complete this code.
 */
int rgbtree::partition(int lo, int hi, int d) 
{
    int p = lo;
    RGBAPixel pix_i;
    RGBAPixel pix_lo;

    for( int i=lo+1; i <= hi; i++ ) {
      pix_i = tree[i];
      pix_lo = tree[lo];

      if(smallerByDim(pix_i, pix_lo, d)) { 
        p++; swap(tree[p], tree[i]); 
        }
    }
    swap(tree[lo], tree[p]);
    return p;

}


/**
 * Helper function to help determine if the nearest neighbor could 
 * be on the other side of the KD tree.
 */
int rgbtree::distToSplit(const RGBAPixel& query, const RGBAPixel& curr, int dimension) const
{
  int query_pos = getDim(query, dimension);
  int curr_pos = getDim(curr, dimension);

  return ((query_pos - curr_pos) * (query_pos - curr_pos));
}

/* 
    Helper function that returns RGBAPixel dimension value.  For d=0,1,2, return 
    pixel r,g,b value
    */
 int rgbtree::getDim(const RGBAPixel& pixel, int d) const
 {
   unsigned char dimVal;

   if (d == 0) {
     dimVal = pixel.r;
   } else if (d == 1) {
     dimVal = pixel.g;
   } else {
     dimVal = pixel.b;
   }

   return (int) dimVal;
 }

  /*
   * Helper function for findNearestNeighbor
   */ 
  RGBAPixel rgbtree::workDown(const RGBAPixel& query, int start, int end, int currMinDist, RGBAPixel nn, int level) const
  {
    while (end - start >= 0) {
      int curr = (start + end) / 2;
      int dist = computeDist(query, tree[curr]);
      if (dist <= currMinDist) {
        currMinDist = dist;
        nn.operator=(tree[curr]);
      }
      int dim = level % 3;
      int distToPlane = distToSplit(query, tree[curr], dim);
      int l = level+1;
      if (smallerByDim(query, tree[curr], dim)) {
        if (currMinDist > distToPlane) {
          nn.operator=(workDown(query, curr+1, end, currMinDist, nn, l));
          currMinDist = computeDist(query, nn);
        }
        end = curr-1;
      } else {
        if (currMinDist > distToPlane) {
          nn.operator=(workDown(query, start, curr-1, currMinDist, nn, l));
          currMinDist = computeDist(query, nn);
        }
        start = curr+1;
      }
      level++;
    }
    return nn;
  }

  /*
   * Helper to compute Euclidean dist squared between two pixels
   */
   int rgbtree::computeDist(const RGBAPixel& first, const RGBAPixel& second) const
   {
     int first_r = getDim(first, 0);
     int first_g = getDim(first, 1);
     int first_b = getDim(first, 2);
     int second_r = getDim(second, 0);
     int second_g = getDim(second, 1);
     int second_b = getDim(second, 2);

     return (first_r - second_r) * (first_r - second_r) + (first_g - second_g) * (first_g - second_g) + (first_b - second_b) * (first_b - second_b);
   }


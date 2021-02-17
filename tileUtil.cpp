


#include "tileUtil.h"

/**
 * Function tile:
 * @param PNG & target: an image to use as base for the mosaic. it's pixels will be
 *                      be replaced by thumbnail images whose average color is close
 *                      to the pixel.
 * @param rgbtree & ss: a kd-tree of RGBAPixels, used as a query structure for
 *                      nearest neighbor search. 
 * @param map<RGBAPixel, string> & photos: a map that takes a color key and returns the
 *                      filename of an image whose average color is that key.
 *
 * returns: a PNG whose dimensions are TILESIZE times that of the target. Each
 * pixel in the target is used as a query to ss.findNearestNeighbor, and the response
 * is used as a key in photos. 
 */

PNG tiler::tile(PNG & target, const rgbtree & ss, map<RGBAPixel,string> & photos){

    PNG mosaic = PNG(target.width()*TILESIZE, target.height()*TILESIZE);
    RGBAPixel * target_pixel;
    RGBAPixel * thumb_pixel;
    RGBAPixel * mosaic_pixel;
    RGBAPixel map_pixel;
    PNG thumb;

    for (unsigned int x=0; x < target.width(); x++) {
        for (unsigned int y=0; y < target.height(); y++) {
            target_pixel = target.getPixel(x, y);
            map_pixel = ss.findNearestNeighbor(*target_pixel);
            string path = photos[map_pixel];
            thumb.readFromFile(path);

            for (unsigned int xt=0; xt < thumb.width(); xt++) {
                for (unsigned int yt=0; yt < thumb.height(); yt++) {
                    thumb_pixel = thumb.getPixel(xt, yt);
                    mosaic_pixel = mosaic.getPixel((x*TILESIZE)+xt, (y*TILESIZE)+yt);
                    *mosaic_pixel = *thumb_pixel;
                }
            }
        }
    }
    return mosaic;
}

/* buildMap: function for building the map of <key, value> pairs, where the key is an
 * RGBAPixel representing the average color over an image, and the value is 
 * a string representing the path/filename.png of the TILESIZExTILESIZE image
 * whose average color is the key.
 * 
 * @param path is the subdirectory in which the tiles can be found. In our examples
 * this is imlib.
 *
*/
map<RGBAPixel, string> tiler::buildMap(string path) {

    map < RGBAPixel, string> thumbs;
    RGBAPixel average;
    for (const auto & entry : fs::directory_iterator(path)) {
        int red = 0;
        int green = 0;
        int blue = 0;
        int count = 0;

        PNG curr; 
        curr.readFromFile(entry.path());
        for (unsigned int x=0; x < curr.width(); x++) {
            for (unsigned int y=0; y < curr.height(); y++) {
                RGBAPixel * curr_pix = curr.getPixel(x, y);
                int curr_r = (int) curr_pix->r;
                int curr_g = (int) curr_pix->g;
                int curr_b = (int) curr_pix->b;
                red += curr_r;
                green += curr_g;
                blue += curr_b;
                count++;
            }
        }
        red = red / count;
        green = green / count;
        blue = blue /count;

        average = RGBAPixel(red, green, blue);
        thumbs.insert({average, entry.path()});
    }
    return thumbs;
}



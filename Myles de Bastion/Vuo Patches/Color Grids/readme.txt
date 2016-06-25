Submitted by Magneson (@MartinusMagneson) on 2016.02.10 05:15
(Just realized this would become a very, very long post although it's barely scratching the surface)

Vuo is very flexible in color handling, so you can more or less figure out which workflow suits you the best. Here are some compositions showing a few, but there are even more ways to do it. I use the HSL node to do a lot of it, because when you get over to palette creation and the sorts, it's a lot easier to deal with degrees when thinking color theory. The conversion is fairly simple to calculate based on the HSL model and a few math nodes.

The base composition is the grid model, since it's conceptually easy, and it involves the Build List node which give you an identifier for the item. Item 1 is in the bottom left corner, max item is in the top right. As all color data is in the 0-1 range initially, you divide 1 by the max number of items in the list, and then multiplies the result by the item number to get its "address".

What might be a bit confusing when building separate color lists (or rather real lists that edit color) is that the lists are separated. This doesn't matter as long as the lists have the same number of items. The important bit though, is to use the hold node triggered by the Build Item port everywhere you have a "crossing", where you edit one list with the other list and events get shuffled around. There might be some unwanted or hard to figure out stuff happening in these comps, as there are some event flow issues that I don't know what's causing. (@Jaymie, would you mind having a look, especially at the events from the Get Window Dimensions node? It seems like even with a hold node in place it struggles with separating)

So here we go. All you need to look at for the color bit is the lime-colored nodes.

1. Magneson_RainbowGrid
Color is calculated within the Build List that generates the grid. It uses a Scale node set from 0 to max item (x axis multiplied with y axis), and with a scaled range of 0-1. This is then fed to the Hue input port of the Make HSL Color node to generate the color. This is then filtered by a Hold node that is triggered by the Build Item port of the grid Build List node (Hold node is in place because the multiplication continuously fires events).

2.Magneson_EnqueuedColor
A Wave node generates values between 0-1. This is fed into an Enqueue node to hold the previous values. The Max Item Count input of the Enqueue node is the total amount of items. A Get Item From List node is then used to get the corresponding value for the item in the grid.

3.Magneson_BuildListColor
To get more finite control over each item in the list, an additional Build List node is used. This generates a new list containing color information (or real numbers to generate colors as is the case with these) that the grid Build List node can use. The fired item number from the list stores the value from the Share Value node to the corresponding item in the list. All are equal in this case, but it is the base concept.

4.Magneson_BuildListEnqueueColor
Changing things up a bit, here the output of the color Build List is connected to a strain of Change Item in List nodes. The data from the Wave node selects which item is to be changed, and the enqueue list delays the selection for the first and second Change Item in List nodes (the last item in the enqueue node is the most recent).

5. Magneson_BuildListCountColor
To get more control over the delay and positions, the Wave node is swapped out for the Count within Range node triggered by a Fire Periodically node. Where the wave method continuously change the position in such small increments that they might overlap if not given enough timed delay, this method changes a full step each time it's triggered. Because of this, the list only needs to be as long as the max item count, and spacing is more intuitive (subtracting from max item gives the step).

6.Magneson_BuildListConditionalColor
For ultimate (and rather complex) control of the coloring, you can use boolean conditions to decide color. Using the Are equal node (the absolute, not sloppy version) you can filter the item number, and change it with the Select Input node (Boolean). Keep in mind that these can be chained for even more complexity, and the item number can of course be modulated to animate it.

7.Magneson_BuildListMultCondCol
The Select Input node also works with the Calculate node to do more comparisons at the same time. Here I used "(Item == A) || (Item == B) || (Item == C) || (Item == D)" to color four items of a definable position. The ABCD inputs sets which item to check for matching, and the Item input takes the current item from the build list. As the inputs are variable, you can animate the grid by changing which item to check for (like the Wave node on input A).

8.Magneson_BuildListCameraColor
Differing quite a bit again, this uses the Build List node to generate the colors (not numbers). It takes the 2d point generated from the grid calculation (but with its own hold nodes) along with the element width to sample the colors from an image (or video in this case) at the position of the item in the grid.

The Get Window Dimensions node isn't playing nice, so there are hardcoded values for the grid. I still hope this can be of some help in understanding some (mine at least) ways of coloring grids and other multiples of stuff in Vuo. I only used the Make HSL Color here without lightness or saturation, and you also have the RGB and Hex coloring nodes which are their own containers of fun (and probably frustration).
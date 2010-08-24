
# Check out revision 24789 of ffmpeg
echo "Checking out FFMPEG"
svn co -r 24789 svn://svn.ffmpeg.org/ffmpeg/trunk ffmpeg >> /dev/null

# Copy the plugin 
# Note:  Libavfilter is only available in the 0.6 branch. Older version of FFMPEG will not work.
echo "Copying src/vf_plugin.c to FFMPEG AVFilter dir..."
cp src/vf_plugin.c  ffmpeg/libavfilter/vf_plugin.c

#
# We need to modify 2 files in libavfilter so that it will include
# the plugin filter when we compile.
#

cd ffmpeg/libavfilter

echo "Applying patches..."
sed '49i\
REGISTER_FILTER (PLUGIN,      plugin,      vf);
' allfilters.c > allfilters.c.tmp

sed '30i\
OBJS-$(CONFIG_PLUGIN_FILTER)                 += vf_plugin.o
' Makefile > Makefile.tmp


mv allfilters.c.tmp allfilters.c
mv Makefile.tmp Makefile
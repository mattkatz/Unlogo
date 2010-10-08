set -e
../../bin/ffmpeg -qscale 2 -i resources/videos/IMG_0076.MOV -vf plugin=unlogo.so:5:262:111:447:92:449:209:261:240 -y share/results/IMG_0076_1.MOV
../../bin/ffmpeg -qscale 2 -i resources/videos/IMG_0076.MOV -vf plugin=unlogo.so:173:250:181:512:234:517:309:250:296 -y share/results/IMG_0076_2.MOV
../../bin/ffmpeg -qscale 2 -i resources/videos/IMG_0077.MOV -vf plugin=unlogo.so:9:70:183:214:119:206:275:52:300 -y share/results/IMG_0077.MOV
../../bin/ffmpeg -qscale 2 -i resources/videos/IMG_0079.MOV -vf plugin=unlogo.so:15:276:193:487:223:475:275:280:254 -y share/results/IMG_0079.MOV
../../bin/ffmpeg -qscale 2 -i resources/videos/IMG_0081.MOV -vf plugin=unlogo.so:8:291:228:372:228:364:254:290:253 -y share/results/IMG_0081.MOV
../../bin/ffmpeg -qscale 2 -i resources/videos/IMG_0082.MOV -vf plugin=unlogo.so:0:113:215:289:215:289:240:111:240 -y share/results/IMG_0082.MOV
../../bin/ffmpeg -qscale 2 -i resources/videos/IMG_0083.MOV -vf plugin=unlogo.so:0:235:209:387:227:393:335:231:333 -y share/results/IMG_0083_1.MOV
../../bin/ffmpeg -qscale 2 -i resources/videos/IMG_0083.MOV -vf plugin=unlogo.so:90:349:116:437:147:452:251:357:235 -y share/results/IMG_0083_2.MOV
../../bin/ffmpeg -qscale 2 -i resources/videos/IMG_0084.MOV -vf plugin=unlogo.so:35:102:167:313:184:315:224:103:219 -y share/results/IMG_0084_1.MOV
../../bin/ffmpeg -qscale 2 -i resources/videos/IMG_0084.MOV -vf plugin=unlogo.so:352:174:252:446:166:448:257:191:312 -y share/results/IMG_0084_2.MOV
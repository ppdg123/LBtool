
%for i=1:size(samples,2);
    id = 'Mossskas141';
    im = imread(sprintf('./LBtool/img/%s.jpg',id));
    sp = segment(im,0.5,50,150);
    dsp = double(sp);
    im = uint8(zeros(size(sp,1),size(sp,2),3));
    im(:,:,1) = mod(uint16(dsp),256);
    dsp = floor(dsp/256);
    im(:,:,2) = mod(uint16(dsp),256);
    dsp = floor(dsp/256);
    im(:,:,3) = mod(uint16(dsp),256);
    imwrite(im,sprintf('./LBtool/img/%s.jpg.png',id));
%end
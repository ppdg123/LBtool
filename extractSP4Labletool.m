
for i=1:size(samples,2);
    sp = samples(i).pixellevel.superpixels;
    dsp = double(sp);
    im = uint8(zeros(size(sp,1),size(sp,2),3));
    im(:,:,1) = mod(uint16(dsp),256);
    dsp = floor(dsp/256);
    im(:,:,2) = mod(uint16(dsp),256);
    dsp = floor(dsp/256);
    im(:,:,3) = mod(uint16(dsp),256);
    fn = samples(i).im;
    md5code = samples(i).desp.md5;
    cmd = sprintf('cp %s ./LBtool/img/%s.jpg',fn,md5code);
    system(cmd);
    imwrite(im,sprintf('./LBtool/img/%s.jpg.png',md5code));
end
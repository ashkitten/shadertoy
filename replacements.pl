use strict;
use warnings;

next if $. == 1;

my @idents = (
    "MapInfo",
    "Material",
    "Ray",
    "box",
    "calcNormal",
    "cameraRay",
    "cameraRight",
    "cameraUp",
    "color",
    "diffuse",
    "direction",
    "distanceFade",
    "eps",
    "fogAmount",
    "globalUp",
    "hit",
    "hitColor",
    "initRayToDirection",
    "initRayToTarget",
    "lightRay",
    "map",
    "mapInfo",
    "material",
    "normal",
    "o1",
    "o2",
    "opIntersect",
    "opSubtract",
    "opUnion",
    "origin",
    "penumbra",
    "plane",
    "position",
    "rad",
    "ray",
    "reflections",
    "reflectivity",
    "resolution",
    "shadow",
    "size",
    "softshadow",
    "specular",
    "specular",
    "sphere",
    "target",
    "trace",
);

my @defines = (
    "1.5",
    "distance",
    "float",
    "normalize",
    "return",
    "vec3",
);

my %replacements = (
    ".0001*2." => ".0002",
    "5+1"      => "6",
    "*.01"     => "/10",
);

my @all = (@idents, @defines);

my @dict = ('a'..'z', 'A'..'Z', 'aa'..'zz');

if ($. == 2) {
    foreach my $i (0..$#defines) {
        print "#define $dict[$i + $#idents + 1] $defines[$i]\n";
    }
}

# remove leading whitespace
s/^\s+//;
# remove newlines except following preprocessor directives
s/\n//g if !/^#/;
# remove whitespace between non-word characters and anything
s/(?<=\W)\s+(?=.)|(?<=.)\s+(?=\W)//g;
# remove leading and trailing 0's
s/\b((?<=[^0]\.)0|0(?=\.\d+))\b//g;

# rename iterator variable to not conflict with others
s/\bi\b/_/g;

foreach my $i (0..$#all) {
    s/\b$all[$i]\b/$dict[$i]/g;
}

foreach my $key (keys %replacements) {
    my $str = quotemeta($key);
    s/$str/$replacements{$key}/g
}

use strict;
use warnings;

# start on second line (print out first line with no changes)
print scalar <>;
my @file = <>;
close STDIN;

my %replacements = (
    "0.0001 * 2.0"        => "0.0002",
    "5 + 1"               => "6",
    "* 0.01"              => "/ 10",
    "vec3(1.0)"           => "vec3(1)",
    "vec3(0.0, 0.0, 0.0)" => "vec3(0)",
    "25.0"                => "25",
);

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

my @all = (@idents, @defines);

my @dict = ('a'..'z', 'A'..'Z', 'aa'..'zz');

foreach my $key (keys %replacements) {
    my $str = quotemeta($key);
    if (!grep /$str/, @file) {
        print STDERR "WARNING: replacement '$key' not found\n";
    }
}

foreach my $ident (@idents) {
    my $str = quotemeta($ident);
    if (!grep /\b$str\b/, @file) {
        print STDERR "WARNING: ident '$ident' not found\n";
    }
}

foreach my $define (@defines) {
    my $str = quotemeta($define);
    if (!grep /\b$str\b/, @file) {
        print STDERR "WARNING: define '$define' not found\n";
    }
}

foreach my $i (0..$#defines) {
    print "#define $dict[$i + $#idents + 1] $defines[$i]\n";
}

foreach (@file) {
    foreach my $key (keys %replacements) {
        my $str = quotemeta($key);
        s/$str/$replacements{$key}/g
    }

    foreach my $i (0..$#all) {
        my $str = quotemeta($all[$i]);
        s/\b$str\b/$dict[$i]/g;
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

    print;
}

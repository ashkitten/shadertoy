use strict;
use warnings;

# start on second line (print out first line with no changes)
print scalar <>;
my @file = <>;
close STDIN;

my %replacements = (
    "5+1"            => "6",
    "*.01"           => "/10",
    "*.5"            => "/2",
    "vec3(1.)"       => "vec3(1)",
    "vec3(.0,.0,.0)" => "vec3(0)",
    "25."            => "25",
    "int _"          => "_",
    "resolution.xy"  => "resolution",
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
    ".0001",
);

my @all = (@idents, @defines);

my @dict = ('a'..'z', 'A'..'Z', 'aa'..'zz');

foreach my $i (0..$#defines) {
    print "#define $dict[$i + $#idents + 1] $defines[$i]\n";
}

# rename iterator variable to not conflict with others
grep s/\bi\b/_/g, @file;

# define iterator variable as global
print "int _;";

# remove leading whitespace
grep s/^\s+//, @file;
# remove newlines except following preprocessor directives
grep s/\n//, grep !/^#/, @file;
# remove whitespace between non-word characters and anything
grep s/(?<=\W)\s+(?=.)|(?<=.)\s+(?=\W)//g, @file;
# remove leading and trailing 0's
grep s/\b((?<=[^0]\.)0|0(?=\.\d+))\b//g, @file;

foreach my $key (keys %replacements) {
    if (!grep /\Q$key\E/, @file) {
        print STDERR "WARNING: '$key' not found\n";
    }
    grep s/\Q$key\E/$replacements{$key}/g, @file;
}

foreach my $i (0..$#all) {
    if (!grep /(?<=\W)\Q$all[$i]\E(?=\W)/, @file) {
        print STDERR "WARNING: '$all[$i]' not found\n";
    }
    grep s/((?<=\W)|\b)\Q$all[$i]\E((?=\W)|\b)/$dict[$i]/g, @file;
}

print @file;

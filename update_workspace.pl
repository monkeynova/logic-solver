#!/usr/bin/env perl

use strict;
use warnings;

use JSON;

my %commit_map;
my $workspace = join '', <>;
while ($workspace =~ m{git_repository\(([^)]+)\)}sg) {
    my $repository = $1;
    my %args = $repository =~ /([a-z]+)\s*=\s*\"(.*?)(?!<\")\"/g;
    
    if ($args{remote} =~ m{https://github.com/(.*)\.git}) {
	my $github_name = $1;
	my $json_str = `curl https://api.github.com/repos/$github_name/commits/master`;
	my $json = JSON::from_json($json_str);
	my $sha = $json->{sha};
	if ($sha) {
	    print STDERR "updating $args{name} ($github_name) => $sha\n";
	    $commit_map{$args{commit}} = $sha;
	} else {
	    print STDERR "cannot update $args{name} ($github_name)!\n";
	}
    }
}

while (my ($pre_sha, $post_sha) = each %commit_map) {
    $workspace =~ s{$pre_sha}{$post_sha};
}

print $workspace;

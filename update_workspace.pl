#!/usr/bin/env perl

use strict;
use warnings;

use JSON;

die "Did you mean $0 < WORKSPACE > WORKSPACE.new" if -t STDIN;

my %commit_map;
my $workspace = join '', <>;
while ($workspace =~ m{git_repository\(([^)]+)\)}sg) {
    my $repository = $1;
    my %args = $repository =~ /([a-z]+)\s*=\s*\"(.*?)(?!<\")\"/g;
    next if $commit_map{$args{commit}};
    
    if ($args{remote} =~ m{git://github.com/(.*)\.git}) {
	my $github_name = $1;
	my $json_str = `curl -s https://api.github.com/repos/$github_name/commits`;
	my $json = JSON::from_json($json_str);
	if (ref($json) ne 'ARRAY') {
	    warn "Cannot find commit for ", $github_name, ": ", $json_str;
	    next;
	}
	my $sha = $json->[0]{sha};
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

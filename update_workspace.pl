#!/usr/bin/env perl

use strict;
use warnings;

use JSON;

die "Did you mean $0 < WORKSPACE > WORKSPACE.new" if -t STDIN;

my %commit_map;
my $workspace = join '', <>;
while ($workspace =~ m{git_repository\(([^)]+)\)}sg) {
  my $repository = $1;
  my %args = $repository =~ /([a-z_]+)\s*=\s*\"(.*?)(?!<\")\"/g;
  next if $commit_map{$args{commit}};
  if ($args{no_update}) {
    warn "Not updating $args{remote}: $args{no_update}\n";
    next;
  }
  
  if ($args{remote} =~ m{git://github.com/(.*)\.git}) {
    my $github_name = $1;
	my $branch = ($args{branch} && "/$args{branch}") // "/master";
    my $json_str = `curl -s https://api.github.com/repos/$github_name/commits$branch`;
    my $json = JSON::from_json($json_str);
    if (ref($json) ne 'HASH') {
        warn "Cannot find commit for ", $github_name, ": ", $json_str;
        next;
    }
    my $sha = $json->{sha};
    if ($sha) {
        if ($args{commit} ne $sha) {
          warn "updating $args{name} ($github_name) => $sha\n";
          $commit_map{$args{commit}} = $sha;
        } else {
          warn "unchanged $args{name}\n";
        }
    } else {
        warn "cannot update $args{name} ($github_name)!\n";
    }
  }
}

while (my ($pre_sha, $post_sha) = each %commit_map) {
    $workspace =~ s{$pre_sha}{$post_sha};
}

print $workspace;

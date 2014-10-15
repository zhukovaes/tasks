#!/usr/bin/perl

#MAIN

#Predefined variables
my $config = { debug => 1, exec_run => 0};

#Students info
my $names = { "ark85" => "Kudashov Arkady",
                "KirillKorolev" => " Kirill Korolev",
                "ivanychev" => "Sergey Ivanovitch",
                "Lev-T" => "Lev Tepljakov",
                "IlyaMarkovMipt" => "Ilya Markov",
                "Atsanda" => "Artem Tsanda",
                "skotti" => "Anastasia Ruzhanskaya",
                "Shishpan" => "Andrew Shishpanov",
                "StrausMG" => "Gorishnii Yuri",
                "Devernua" => "Kukuev Alexander" };
my @tasks = ( "example_task", "task1" );
                
my $clone_prefix = "https://github.com/";
my $clone_suffix = "/tasks.git";

my $results = {};

#Parse input options
if (!(@ARGV))
{
    PrintHelp();
}

while(@ARGV)
{
    $inword = shift @ARGV;
                                    
    if ( $inword eq '-help' || $inword eq '-h')
    {
        PrintHelp();
    } elsif ( $inword eq '-info')
    {
        $config->{info} = 1;
    } elsif ( $inword eq '-wipe')
    {
        $config->{wipe} = 1;
    } elsif ( $inword eq '-only_task')
    {
        $config->{only_task} = shift @ARGV;
    }  elsif ( $inword eq '-only_student')
    {
        $config->{only_student} = shift @ARGV;
    } 
}

if ( $config->{info})
{
    PrintInfo();
    exit 0;
}

#Perform actions
foreach my $name ( keys %{$names})
{
    if ( !defined( $config->{only_student}) || $config->{only_student} eq $name)
    {
        Actions( $name);
    }
} 
    
    
sub Actions
{
    my ($name) = @_;
    
    #check dir/make dir
    PrepareDir( $name);
    
    #clone/update repo
    PrepareRepo( $name);
    
    # build and run tasks
    foreach my $task (@tasks)
    {
        if ( !defined( $config->{only_task}) || $config->{only_task} eq $task)
        {
            ProcessTask( $name, $task);
        }
    }
}

sub PrepareDir
{
    my ($name) = @_;
    
    if ( $config->{wipe} )
    {
        Run("rm $name");
    }
    if ( !(-d $name))
    {
        Run( "mkdir $name");
    }
}

sub PrepareRepo
{
    my ($name) = @_;

    my $target = $name."/tasks";
    
    if ( !(-d $target))
    {
        Run( "git clone ".CloneURL($name)." $target");
    } else
    {
        #check for local modifications and revert them
        
        Run("cd $target; git pull");
    }
}

sub ProcessTask
{
    my ($name, $task) = @_;
    
    my $target_dir = $name."/tasks/".$task;
    my $res = {};
    $results->{$name}->{$task} = $res;
    
    #Run build
    Run("cd $target_dir; make 1>make.out 2>make.err");
    
    #Parse build logs
    
    #Run testing
    Run("cd $target_dir; ./bin/utestd");
    
    #Parse run logs
    
}
    
sub CloneURL
{
    my ($name) = @_;
    
    if ( !defined( $names->{$name}))
    {
        die "Unknown name $name\n";
    }
    
    return $clone_prefix.$name.$clone_suffix;
}


sub PrintHelp
{
    print "tts.pl - Task Testing System\n";
}

sub PrintInfo
{
    print "Tasks:\n";
    foreach my $task (@tasks)
    {
        if ( !defined( $config->{only_task}) || config->{only_task} eq $task)
        {
            print "    $task\n";
        }
    }
    print "\n";
    
    print "Students:\n";
    foreach my $acc ( keys %{$names})
    {
        print "    $acc    ".$names->{$acc}."     ".CloneURL($acc)."\n";
    }
}

sub PrintResults
{
    foreach my $acc ( keys %{$names})
    {
        print "$acc ".$names->{$acc}." ".CloneURL($acc)."\n";
    }
}

sub NewResult
{
    my ($name) = @_;
    
    if ( !defined( $name))
    {
        my $rec = {};
        $results->{$name} = {};
        
    }
}

sub Run
{
    my ($cmd) = @_;
    
    if ( $config->{exec_run})
    {
        system ($cmd);
    } else
    {
        print $cmd."\n";
    }
}

sub DPrint
{
    my ($str) = @_;
    if ( $config->{debug})
    {
        print STDERR $str;
    }
}
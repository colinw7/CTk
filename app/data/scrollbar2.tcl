# called by the listbox
proc yset {widgets master sb args} {
  if {$master eq "master"} {
    # only the master sets the scrollbar
    $sb set {*}$args
    set wl [lrange $widgets 1 end]
  } else {
    set wl [lrange $widgets 0 0]
  }
  yview $wl moveto [lindex $args 0]
}

# called by the scrollbar
proc yview {widgets args} {
  foreach w $widgets {
    # puts "$w yview $args"
    $w yview {*}$args
  }
}

listbox .lb1 -relief raised -borderwidth 2 -listvariable files
listbox .lb2 -relief raised -borderwidth 2 -listvariable files

scrollbar .sb -command ".files yview"

set widgets [list .lb1 .lb2]

.lb1 configure -yscrollcommand [list yset $widgets master .sb]
.lb2 configure -yscrollcommand [list yset $widgets slave  .sb]
.sb  configure -command [list yview $widgets]

grid .lb1 -row 0 -column 0 -sticky nsew
grid .lb2 -row 0 -column 1 -sticky nsew
grid .sb  -row 0 -column 2 -sticky ns

grid rowconfigure    . 0 -weight 1
grid columnconfigure . 0 -weight 1
grid rowconfigure    . 1 -weight 1
grid columnconfigure . 1 -weight 1

set files [lsort -dictionary [glob -directory . -tails *]]

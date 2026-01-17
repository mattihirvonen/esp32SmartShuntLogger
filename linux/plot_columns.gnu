# Set output to PNG image
#set terminal pngcairo size 1600,900 enhanced font 'Arial,10'
#set output 'plot.png'

# Set labels and title
set title "Example: Plotting Specific Columns from a Data File"
set xlabel  "Time [hours]"
set ylabel  "Voltage [V]"
set y2label "Current [A]"

# Enable y2 axis
set y2tics
set ytics  nomirror  # Avoid duplicate tics on left
set y2tics nomirror  # Avoid duplicate tics on right

# Optional: set ranges (auto if omitted)
#set xrange  [9.5:10]
set  yrange  [11:15]
set  y2range [0:4]
#set  y2range [-0.5:3.5]

# Enable grid
set grid

# types: lines, linespoints
# Plot: using column 1 as X, and columns 2, 3, 4 as Y
plot 'data.txt' using 1:2           with lines title 'Hupi [V]', \
     'data.txt' using 1:3           with lines title 'Startti [V]', \
     'data.txt' using 1:4 axes x1y2 with lines title 'Virta [A]'

# Pause to show screen
pause mouse close

<main class="col-md markdown-body">

<h1 id="recover">Recover</h1>

<p>Implement a program that recovers JPEGs from a forensic image, per the below.</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>$ ./recover card.raw
</code></pre></div></div>

<h2 id="background">Background</h2>

<p>In anticipation of this problem, we spent the past several days taking photos of people we know, all of which were saved on a digital camera as JPEGs on a memory card. (Okay, it’s possible we actually spent the past several days on Facebook instead.) Unfortunately, we somehow deleted them all! Thankfully, in the computer world, “deleted” tends not to mean “deleted” so much as “forgotten.” Even though the camera insists that the card is now blank, we’re pretty sure that’s not quite true. Indeed, we’re hoping (er, expecting!) you can write a program that recovers the photos for us!</p>

<p>Even though JPEGs are more complicated than BMPs, JPEGs have “signatures,” patterns of bytes that can distinguish them from other file formats. Specifically, the first three bytes of JPEGs are</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>0xff 0xd8 0xff
</code></pre></div></div>

<p>from first byte to third byte, left to right. The fourth byte, meanwhile, is either <code class="highlighter-rouge">0xe0</code>, <code class="highlighter-rouge">0xe1</code>, <code class="highlighter-rouge">0xe2</code>, <code class="highlighter-rouge">0xe3</code>, <code class="highlighter-rouge">0xe4</code>, <code class="highlighter-rouge">0xe5</code>, <code class="highlighter-rouge">0xe6</code>, <code class="highlighter-rouge">0xe7</code>, <code class="highlighter-rouge">0xe8</code>, <code class="highlighter-rouge">0xe9</code>, <code class="highlighter-rouge">0xea</code>, <code class="highlighter-rouge">0xeb</code>, <code class="highlighter-rouge">0xec</code>, <code class="highlighter-rouge">0xed</code>, <code class="highlighter-rouge">0xee</code>, or <code class="highlighter-rouge">0xef</code>. Put another way, the fourth byte’s first four bits are <code class="highlighter-rouge">1110</code>.</p>

<p>Odds are, if you find this pattern of four bytes on media known to store photos (e.g., my memory card), they demarcate the start of a JPEG. To be fair, you might encounter these patterns on some disk purely by chance, so data recovery isn’t an exact science.</p>

<p>Fortunately, digital cameras tend to store photographs contiguously on memory cards, whereby each photo is stored immediately after the previously taken photo. Accordingly, the start of a JPEG usually demarks the end of another. However, digital cameras often initialize cards with a FAT file system whose “block size” is 512 bytes (B). The implication is that these cameras only write to those cards in units of 512 B. A photo that’s 1 MB (i.e., 1,048,576 B) thus takes up 1048576 ÷ 512 = 2048 “blocks” on a memory card. But so does a photo that’s, say, one byte smaller (i.e., 1,048,575 B)! The wasted space on disk is called “slack space.” Forensic investigators often look at slack space for remnants of suspicious data.</p>

<p>The implication of all these details is that you, the investigator, can probably write a program that iterates over a copy of my memory card, looking for JPEGs’ signatures. Each time you find a signature, you can open a new file for writing and start filling that file with bytes from my memory card, closing that file only once you encounter another signature. Moreover, rather than read my memory card’s bytes one at a time, you can read 512 of them at a time into a buffer for efficiency’s sake. Thanks to FAT, you can trust that JPEGs’ signatures will be “block-aligned.” That is, you need only look for those signatures in a block’s first four bytes.</p>

<p>Realize, of course, that JPEGs can span contiguous blocks. Otherwise, no JPEG could be larger than 512 B. But the last byte of a JPEG might not fall at the very end of a block. Recall the possibility of slack space. But not to worry. Because this memory card was brand-new when I started snapping photos, odds are it’d been “zeroed” (i.e., filled with 0s) by the manufacturer, in which case any slack space will be filled with 0s. It’s okay if those trailing 0s end up in the JPEGs you recover; they should still be viewable.</p>

<p>Now, I only have one memory card, but there are a lot of you! And so I’ve gone ahead and created a “forensic image” of the card, storing its contents, byte after byte, in a file called <code class="highlighter-rouge">card.raw</code>. So that you don’t waste time iterating over millions of 0s unnecessarily, I’ve only imaged the first few megabytes of the memory card. But you should ultimately find that the image contains 50 JPEGs.</p>

<h2 id="getting-started">Getting Started</h2>

<p>Here’s how to download this problem’s “distribution code” (i.e., starter code) into your own CS50 IDE. Log into <a href="https://cs50.io/">CS50 IDE</a> and then, in a terminal window, execute each of the below.</p>

<ol>
  <li>Execute <code class="highlighter-rouge">cd</code> to ensure that you’re in <code class="highlighter-rouge">~/</code> (i.e., your home directory).</li>
  <li>If you haven’t already, execute <code class="highlighter-rouge">mkdir pset4</code> to make (i.e., create) a directory called <code class="highlighter-rouge">pset4</code> in your home directory.</li>
  <li>Execute <code class="highlighter-rouge">cd pset4</code> to change into (i.e., open) your <code class="highlighter-rouge">pset4</code> directory.</li>
  <li>Execute <code class="highlighter-rouge">wget https://cdn.cs50.net/2019/fall/psets/4/recover/recover.zip</code> to download a (compressed) ZIP file with this problem’s distribution.</li>
  <li>Execute <code class="highlighter-rouge">unzip recover.zip</code> to uncompress that file.</li>
  <li>Execute <code class="highlighter-rouge">rm recover.zip</code> followed by <code class="highlighter-rouge">yes</code> or <code class="highlighter-rouge">y</code> to delete that ZIP file.</li>
  <li>Execute <code class="highlighter-rouge">ls</code>. You should see a directory called <code class="highlighter-rouge">recover</code>, which was inside of that ZIP file.</li>
  <li>Execute <code class="highlighter-rouge">cd recover</code> to change into that directory.</li>
  <li>Execute <code class="highlighter-rouge">ls</code>. You should see this problem’s distribution, including <code class="highlighter-rouge">card.raw</code> and <code class="highlighter-rouge">recover.c</code>.</li>
</ol>

<h2 id="specification">Specification</h2>

<p>Implement a program called <code class="highlighter-rouge">recover</code> that recovers JPEGs from a forensic image.</p>

<ul>
  <li data-marker="*">Implement your program in a file called <code class="highlighter-rouge">recover.c</code> in a directory called <code class="highlighter-rouge">recover</code>.</li>
  <li data-marker="*">Your program should accept exactly one command-line argument, the name of a forensic image from which to recover JPEGs.</li>
  <li data-marker="*">If your program is not executed with exactly one command-line argument, it should remind the user of correct usage, and <code class="highlighter-rouge">main</code> should return <code class="highlighter-rouge">1</code>.</li>
  <li data-marker="*">If the forensic image cannot be opened for reading, your program should inform the user as much, and <code class="highlighter-rouge">main</code> should return <code class="highlighter-rouge">1</code>.</li>
  <li data-marker="*">Your program, if it uses <code class="highlighter-rouge">malloc</code>, must not leak any memory.</li>
</ul>

<h2 id="usage">Usage</h2>

<p>Your program should behave per the examples below.</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>$ ./recover
Usage: ./recover image
</code></pre></div></div>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>$ ./recover card.raw
</code></pre></div></div>

<h2 id="hints">Hints</h2>

<p>Keep in mind that you can open <code class="highlighter-rouge">card.raw</code> programmatically with <code class="highlighter-rouge">fopen</code>, as with the below, provided <code class="highlighter-rouge">argv[1]</code> exists.</p>

<div class="language-c highlighter-rouge"><div class="highlight"><pre class="highlight"><code><span class="kt">FILE</span> <span class="o">*</span><span class="n">file</span> <span class="o">=</span> <span class="n">fopen</span><span class="p">(</span><span class="n">argv</span><span class="p">[</span><span class="mi">1</span><span class="p">],</span> <span class="s">"r"</span><span class="p">);</span>
</code></pre></div></div>
<p>When executed, your program should recover every one of the JPEGs from <code class="highlighter-rouge">card.raw</code>, storing each as a separate file in your current working directory. Your program should number the files it outputs by naming each <code class="highlighter-rouge">###.jpg</code>, where <code class="highlighter-rouge">###</code> is three-digit decimal number from <code class="highlighter-rouge">000</code> on up. (Befriend <a href="https://man.cs50.io/3/sprintf"><code class="highlighter-rouge">sprintf</code></a>.) You need not try to recover the JPEGs’ original names. To check whether the JPEGs your program spit out are correct, simply double-click and take a look! If each photo appears intact, your operation was likely a success!</p>

<p>Odds are, though, the JPEGs that the first draft of your code spits out won’t be correct. (If you open them up and don’t see anything, they’re probably not correct!) Execute the command below to delete all JPEGs in your current working directory.</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>$ rm *.jpg
</code></pre></div></div>

<p>If you’d rather not be prompted to confirm each deletion, execute the command below instead.</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>$ rm -f *.jpg
</code></pre></div></div>

<p>Just be careful with that <code class="highlighter-rouge">-f</code> switch, as it “forces” deletion without prompting you.</p>

<p>If you’d like to create a new type to store a byte of data, you can do so via the below, which defines a new type called <code class="highlighter-rouge">BYTE</code> to be a <code class="highlighter-rouge">uint8_t</code> (a type defined in <code class="highlighter-rouge">stdint.h</code>, representing an 8-bit unsigned integer).</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>typedef uint8_t BYTE;
</code></pre></div></div>

<p>Keep in mind, too, that you can read data from a file using <a href="https://man.cs50.io/3/fread"><code class="highlighter-rouge">fread</code></a>, which will read data from a file into a location in memory and return the number of items successfully read from the file.</p>

<h2 id="testing">Testing</h2>

<p>Execute the below to evaluate the correctness of your code using <code class="highlighter-rouge">check50</code>. But be sure to compile and test it yourself as well!</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>check50 cs50/problems/2020/x/recover
</code></pre></div></div>

<p>Execute the below to evaluate the style of your code using <code class="highlighter-rouge">style50</code>.</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>style50 recover.c
</code></pre></div></div>

<h2 id="how-to-submit">How to Submit</h2>

<p>Execute the below, logging in with your GitHub username and password when prompted. For security, you’ll see asterisks (<code class="highlighter-rouge">*</code>) instead of the actual characters in your password.</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>submit50 cs50/problems/2020/x/recover
</code></pre></div></div>

</main>

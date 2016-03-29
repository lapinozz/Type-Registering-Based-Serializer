# Type-Registering-Based-Serializer

This is a project I wanted to do to see if some ideas I had would work in practice. For a while I was thinking of how I could bind C++ class into Roboto (My own scripting language) and the best way I thought of doing so was by "registering" the class and its members, I think most scripting library that offer this feature do it like so. So I built this protoptype to see if the implementation I was thinking about would work. This serializer has multiple flaws and is not completly finished, some part of the code is quite a mess and there's still multiple compile warning. Nontheless I was able to see how to ameliorate the design and I'm quite confident that when it will be time to add thoses feature to Roboto it should be much more simple. It was also an occasion get used to Unit Tests.

# How it work

{% highlight cpp %}
char* i = (char*)"((2-3+2*(3-6)-1)/2.5)*(5+3^2)";using F=float;F z();F y(F a=z()){return*i==94?i++,y(pow(a,z())):a;}F x(F a=y()){return*i-47&&*i-42?a:x(*i++-47?a*y():a/y());}F w(F a=x()){return*i-45&&*i-43?a:w(a+x());}F z(){F r;return*i==40?i++,r=w(),i++,r:strtof(i,&i);}int main(){std::cout<<w();}
{% endhighlight %}

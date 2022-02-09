function legend({
	svgg,
	color,
	title,
	tickSize = 6,
	width = 320,
	height = 44 + tickSize,
	marginTop = 18,
	marginRight = 0,
	marginBottom = 16 + tickSize,
	marginLeft = 0,
    ticks = width / 128,
	tickFormat,
	tickValues
} = {}) {

	let x = Object.assign(color.copy()
		.interpolator(d3.interpolateRound(marginLeft, width - marginRight)), 
		{range() { return [marginLeft, width - marginRight]; }}
	);

	svgg.append("image")
		.attr("x", marginLeft)
		.attr("y", marginTop)
		.attr("width", width - marginLeft - marginRight)
		.attr("height", height - marginTop - marginBottom)
		.attr("preserveAspectRatio", "none")
		.attr("xlink:href", ramp(color.interpolator()).toDataURL());

	if (!x.ticks) {
		if (tickValues === undefined) {
			const n = Math.round(ticks + 1);
			tickValues = d3.range(n).map(i => d3.quantile(color.domain(), i / (n - 1)));
			alert(tickValues);
		}
		if (typeof tickFormat !== "function") {
			tickFormat = d3.format(tickFormat === undefined ? ",f" : tickFormat);
			alert(tickFormat);
		}
	}

	svgg.append("g")
		.attr("transform", `translate(0, ${height - marginBottom})`)
		.call(d3.axisBottom(x)
			.ticks(ticks, typeof tickFormat === "string" ? tickFormat : undefined)
			.tickFormat(typeof tickFormat === "function" ? tickFormat : undefined)
			.tickSize(tickSize)
			.tickValues(tickValues))
		.call(g => g.selectAll(".tick line").attr("y1", marginTop + marginBottom - height))
		.call(g => g.select(".domain").remove())
		.call(g => g.append("text")
			.attr("x", marginLeft)
			.attr("y", marginTop + marginBottom - height - 6)
			.attr("fill", "currentColor")
			.attr("text-anchor", "start")
			.attr("font-weight", "bold")
			.text(title));
}

function ramp(color, n = 256) {
	const canvas = document.createElement('canvas');
	const context = canvas.getContext("2d");
	d3.select(canvas).attr("width", n)
		.attr("height", 1);
	for (let i = 0; i < n; ++i) {
		context.fillStyle = color(i / (n - 1));
		context.fillRect(i, 0, 1, 1);
	}
	return canvas;
}
